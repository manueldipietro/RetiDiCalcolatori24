/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 buffer used for reception and parsing of resp2 messages
*/

#include "resp2_buffer.h"

//This function create a new Resp2 buffer object
Resp2_buffer* resp2_buffer_new(int lenght, int socket_fd){
    Resp2_buffer* buffer = malloc(sizeof(Resp2_buffer));
    buffer->buffer = malloc(sizeof(char)*lenght);
    buffer->lenght = lenght;
    buffer->received = 0;
    buffer->pos = 0;
    buffer->socket_fd = socket_fd;
    return buffer;
}

//This function delete a Resp2 buffer
void resp2_buffer_delete(Resp2_buffer* buffer){
    free(buffer->buffer);
    free(buffer);
    return;
}

//This function receive a Resp2 message and copy it into the Resp2 buffer
int resp2_buffer_receive(Resp2_buffer* buffer){
    buffer->received = resp2_tcp_socket_read(buffer->socket_fd, buffer->buffer, buffer->lenght);
    if(buffer->received < 0){
        resp2_print_error(__FILE__, __LINE__, __func__, "Buffer can't receiving from socket!");
        return -1;
    }
    buffer->pos = 0;
    return buffer->received;
}
