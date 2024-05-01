/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "+<value>\r\n"
*/
#pragma once

#include <string.h>

#include "resp2_buffer.h"

//Resp2 string type definition
typedef struct Resp2_string{
    char* data;
    int lenght;
}Resp2_string;

//This function create a new Resp2 string object
Resp2_string* resp2_string_new(char* data, int lenght);

//This function append a bufferized string to a Resp2 string object
Resp2_string* resp2_string_append(Resp2_string* string, char* data, int lenght);

//This function delete a Resp2 string object
void resp2_string_delete(Resp2_string* string);

//This function convert a Resp2 string to a C string, ready to be sended
int resp2_string_toStr(Resp2_string* string, char** str_to_send);

//This function parse a Resp2 string from a Resp2 buffer
Resp2_string* resp2_string_parser(Resp2_buffer* buffer);

