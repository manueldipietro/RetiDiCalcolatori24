/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 error type "-<value>\r\n"
*/
#pragma once

#include <string.h>

#include "resp2_buffer.h"

#define RESP2_ERROR_GENERIC     "ERR "
#define RESP2_ERROR_WRONGTYPE   "WRONGTYPE "

//Resp2 error type definition
typedef struct Resp2_error{
    char* data;
    int lenght;
}Resp2_error;

//This function create a new Resp2 error object
Resp2_error* resp2_error_new(const char* error_type, char* message, int message_len);

//This function append a bufferized error to a Resp2 error object
Resp2_error* resp2_error_append(Resp2_error* error, char* data, int lenght);

//This function delete a Resp2 error object
void resp2_error_delete(Resp2_error* error);

//This function convert a Resp2 error to a C string, ready to be sended
int resp2_error_toStr(Resp2_error* error, char** str_to_send);

//This function parse a Resp2 error from a Resp2 buffer
Resp2_error* resp2_error_parser(Resp2_buffer* buffer);
