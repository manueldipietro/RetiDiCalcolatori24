/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 socket implementation
*/

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "resp2_utility.h"

#define CON_QUEUE_SZ    5

//This function create a new tcp server socket listening on the passed port number
int resp2_tcp_socket_open_server(int port);

//This function accept a new connection from a tcp server socket and return the client socket descriptor 
int resp2_tcp_socket_accept_connection(int server_socket_fd);

//This function read from a client socket
int resp2_tcp_socket_read(int client_socket_fd, char* message, int message_max_len);

//This function write on a client socket
int resp2_tcp_socket_write(int client_socket_fd, char* message, int message_len);

//This function close a socket
int resp2_tcp_socket_close(int socket_fd);