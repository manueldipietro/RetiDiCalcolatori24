/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "$<len>\r\n<value>\r\n"
*/
#pragma once

#include "malloc.h"
#include "string.h"

#include "resp2_buffer.h"
#include "resp2_utility.h"
#include "resp2_number.h"

typedef struct Resp2_bulkstring{
    char* data;
    int lenght;
}Resp2_bulkstring;

//This function create a new Resp2 bulkstring object
Resp2_bulkstring* resp2_bulkstring_new(char* data, long lenght);

//This function delete a Resp2 bulkstring object
void resp2_bulkstring_delete(Resp2_bulkstring* bulkstring);

//This function convert a Resp2 bulkstring to a C string, ready to be sended
int resp2_bulkstring_toStr(Resp2_bulkstring* bulkstring, char** str_to_send);

//This function parse a Resp2 bulkstring from a Resp2 buffer
Resp2_bulkstring* resp2_bulkstring_parser(Resp2_buffer* buffer);

//This function compare two Resp2 bulkstring
int resp2_bulkstring_cmp(const void* _e1, const void* _e2);