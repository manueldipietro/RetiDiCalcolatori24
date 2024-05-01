/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 socket implementation
*/
#include "resp2_tcp_socket.h"


//This function create a new tcp server socket listening on the passed port number
int resp2_tcp_socket_open_server(int port){
    int server_socket_fd;
    struct sockaddr_in server_address;

    //Create a new IPv4 TCP socket
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket_fd < 0){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on open server socket.");
        return -1;
    }

    //Assign any address to the socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if( bind(server_socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)) == -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on bind server socket.");
        return -1;
    }

    //Listens to the socket for new connections
    if( listen(server_socket_fd, CON_QUEUE_SZ) == -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on listen to server socket.");
        return -1;
    }

    return server_socket_fd;
}

//This function accept a new connection from a tcp server socket and return the client socket descriptor 
int resp2_tcp_socket_accept_connection(int server_socket_fd){
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if(client_socket_fd < 0){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on accept new connection from server socket.");
        return -1;
    }

    return client_socket_fd;
}

//This function read from a client socket
int resp2_tcp_socket_read(int client_socket_fd, char* message, int message_max_len){
    int ret = recv(client_socket_fd, message, message_max_len, 0);
    if(ret == -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on read from socket.");
        return -1;
    }
    return ret;
}

//This function write on a client socket
int resp2_tcp_socket_write(int client_socket_fd, char* message, int message_len){
    int ret = write(client_socket_fd, message, message_len);
    if(ret == -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on write on socket.");
        return -1;
    }
    return ret;
}


//This function close a socket
int resp2_tcp_socket_close(int socket_fd){
    int ret = shutdown(socket_fd, SHUT_RDWR);
    if(ret == -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Error on shutdown socket.");
        return -1;
    }
    return ret;
}