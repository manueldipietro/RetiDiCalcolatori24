/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 number type "+<value>\r\n"
*/
#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "resp2_buffer.h"


typedef struct Resp2_number{
    char* data;
    int lenght;
}Resp2_number;

//This function create a new Resp2 number object
Resp2_number* resp2_number_new(char* data, int lenght);

//This function append a bufferized number to a Resp2 number object
Resp2_number* resp2_number_append(Resp2_number* number, char* data, int lenght);

//This function delete a Resp2 number object
void resp2_number_delete(Resp2_number* number);

//This function convert a Resp2 number to a C string, ready to be sended
int resp2_number_toStr(Resp2_number* number, char** str_to_send);

//This function parse a Resp2 number from a Resp2 buffer
Resp2_number* resp2_number_parser(Resp2_buffer* buffer);

//Convert a resp 2 number to a C long number
long resp2_number_toLong(Resp2_number* number);