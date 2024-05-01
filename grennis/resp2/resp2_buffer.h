/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 buffer used for reception and parsing of resp2 messages
*/

#pragma once

#include <stdlib.h>

#include "resp2_utility.h"
#include "resp2_tcp_socket.h"

//Resp2 buffer type definition
typedef struct Resp2_buffer{
    int socket_fd;
    
    char* buffer;
    int lenght;
    int received;
    int pos;
}Resp2_buffer;

//This function create a new Resp2 buffer object
Resp2_buffer* resp2_buffer_new(int lenght, int socket_fd);

//This function delete a Resp2 buffer
void resp2_buffer_delete(Resp2_buffer* buffer);

//This function receive a Resp2 message and copy it into the Resp2 buffer
int resp2_buffer_receive(Resp2_buffer* buffer);

