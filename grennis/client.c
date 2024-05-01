#include "client.h"

void* client_process(void* args);

Client* client_new(int id, int socket_fd, TDBMS* tdbms, Expirer_manager* expirer_manager){
    int ret = 0;
    Client* client = malloc(sizeof(Client));
    
    client->client_id = id;

    client->request_number = 0;
    client->respons_number = 0;
    ret = pthread_cond_init(&client->send_condition, NULL);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during client cond_var creation!");
        exit(-1);
    }
    ret = pthread_mutex_init(&client->send_mutex, NULL);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during client mutex creation!");
        exit(-1);
    }


    client->socket_fd = socket_fd;
    int socket_readclosed = 0;

    client->expirer_manager = expirer_manager;
    client->tdbms = tdbms;

    client->lib_name = NULL;
    client->lib_ver = NULL;
    ret = pthread_mutex_init(&client->connection_variable_mutex, NULL);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during client mutex creation!");
        exit(-1);
    }


    ret = pthread_create(&(client->thread_descriptor), 0, client_process, client);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during server thread creation!");
        exit(-1);
    }

    return client;
}

void* client_process(void* args ){
    int ret;
    Client* client = (Client*) args;

    Resp2_buffer* buffer = resp2_buffer_new(BUFFER_SIZE, client->socket_fd);
    while(1){
        Resp2_generics* parsed = resp2_parser(buffer);
        if(parsed != NULL){
            if( resp2_generics_get_type(parsed) == RESP2_ARRAY){
                printf("\n\nCOMANDO PASSATO: ");
                char* tostrTest;
                int x = resp2_array_toStr((Resp2_array*) resp2_generics_get_data(parsed), &tostrTest);
                for(int i=0; i<x; i++){
                    printf("%c", tostrTest[i]);
                }
                printf("\n");
                free(tostrTest);
                
                
                Command* command = command_parser((Resp2_array*) resp2_generics_get_data(parsed), client->request_number);           

                client->request_number += 1;
                pthread_t worker;
                switch (command->command_id){
                    case UNRECOGNIZED:
                        pthread_create(&worker, NULL, command_worker_unrecognized, command_worker_args_new(client, command));                  
                        break;
                    case CLIENT_SETINFO:
                        pthread_create(&worker, NULL, command_worker_clientinfo_set, command_worker_args_new(client, command));                  
                        break;
                    case GET:
                        pthread_create(&worker, NULL, command_worker_get, command_worker_args_new(client, command));                  
                        break;
                    case SET:
                        pthread_create(&worker, NULL, command_worker_set, command_worker_args_new(client, command));
                        break;
                    default:
                        break;
                }
                printf("Risposta inviata!\n");
            }
            resp2_generics_delete(parsed);
        }
        else break;
    }
    resp2_buffer_delete(buffer);

    printf("Il client ha chiuso la connessione\n");
    shutdown(client->socket_fd, SHUT_RD);
    client->socket_readclosed = 1;
    pthread_exit(NULL); 
}

void client_delete(Client* client){
    pthread_cond_destroy(&client->send_condition);
    pthread_mutex_destroy(&client->send_mutex);
    pthread_mutex_destroy(&client->connection_variable_mutex);
    free(client);
    pthread_exit(NULL);
}

void client_send_response(Client* client, int  my_sequence, char* string_to_send, int len){
    int ret;

    int e_xit = 0;
    ret = pthread_mutex_lock(&client->send_mutex);
    if(ret!=0){
        print_error(__FILE__, __LINE__, __func__, "Error during getting lock for client send response !");
        exit(-1);
    }
    do{
        e_xit = (my_sequence == client->respons_number ? 1 : 0);
        if(!e_xit){
            pthread_cond_wait(&client->send_condition, &client->send_mutex);
            if(ret!=0){
                print_error(__FILE__, __LINE__, __func__, "Error during condition_variable wait for client send response !");
                exit(-1);
            }
        }
    }while(!e_xit);
         
    ret = resp2_tcp_socket_write(client->socket_fd, string_to_send, len);
    if(ret != len){
        print_error(__FILE__, __LINE__, __func__, "Error during response send!");
        exit(-1);
    }

    client->respons_number += 1;
    ret = pthread_mutex_unlock(&client->send_mutex);
    if(ret){
        print_error(__FILE__, __LINE__, __func__, "Error during condition variable unlock for client send response !");
        exit(-1);
    }
    return;
}



void client_set_libname(Client* client, Resp2_bulkstring* lib_name){
    pthread_mutex_lock(&client->connection_variable_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    client->lib_name = resp2_bulkstring_new(lib_name->data, lib_name->lenght);
    pthread_mutex_unlock(&client->connection_variable_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    return;
}

void client_set_libver(Client* client, Resp2_bulkstring* lib_ver){
    pthread_mutex_lock(&client->connection_variable_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    client->lib_ver = resp2_bulkstring_new(lib_ver->data, lib_ver->lenght);
    pthread_mutex_unlock(&client->connection_variable_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    return;
}

//Search by id
int client_cmp_byid(void* e1, void* e2){
    Client* c1 = (Client*) e1;
    Client* c2 = (Client*) e2;
    
    if(c1->client_id > c2->client_id)
        return 1;
    if(c1->client_id == c2->client_id)
        return 0;
    return -1;
}

//Search if is closed
int is_ended_client(const void* e1, const void* e2){
    if(e1 != NULL && e2!=NULL){
        print_error(__FILE__, __LINE__, __func__, "Error with is_ended_client parameters!");
        exit(-1);
    }
    if(e1 != NULL){
        Client* client = (Client*) e1;
        if(client->socket_readclosed == 1)
            if(client->request_number == client->respons_number)
                return 0;
        return -1;
    }
    if(e2 != NULL){
        Client* client = (Client*) e2;
        if(client->socket_readclosed == 1)
            if(client->request_number == client->respons_number)
                return 0;
        return -1;
    }
    return -1;
}

