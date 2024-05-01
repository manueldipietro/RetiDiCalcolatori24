#include "server.h"

void server_process(Server* server);

//Function that create a new server
void server_new(int port, int max_con, TDBMS* tdbms, Expirer_manager* expirer_manager){
    Server* server = malloc(sizeof(Server));
    
    server->server_port = port;
    server->maximum_connection_number = max_con;
    server->last_connection_id = 0;
    
    server->socket_fd = resp2_tcp_socket_open_server(port);
    if(server->socket_fd < 0){
        print_error(__FILE__, __LINE__, __func__, "Error during server socket creation!");
        exit(-1);
    }

    server->clients = sclTS_new();
    server->tdbms = tdbms;
    server->expirer_manager = expirer_manager;

    server_process(server);

    return;
}

void server_process(Server* server){
    while(1){
        if(sclTS_element_number(server->clients) < server->maximum_connection_number){
            //Accepting new connection    
            int new_client_socket_fd = resp2_tcp_socket_accept_connection(server->socket_fd);
            if(new_client_socket_fd < 0){
                print_error(__FILE__, __LINE__, __func__, "Error during client connection accept!");
                exit(-1);
            }
            Client* client = client_new(server->last_connection_id, new_client_socket_fd, server->tdbms, server->expirer_manager);
            sclTS_append(server->clients, client);
            server->last_connection_id += 1;
            //Deleting died client
            /*Client* finded;
            do{
                finded = sclTS_qr(server->clients, NULL, is_ended_client);
                printf("RIMOZIONE CLIENT: %p.\n", finded);
                if(finded != NULL)
                    client_delete(finded);
            }while(finded != NULL);*/
        }
    }
    return;
}