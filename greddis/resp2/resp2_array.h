/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "*<len>\r\n<element-1>\r\n<elemenet-2>\r\n..."
*/
#pragma once

#include <stdlib.h>

#include "resp2_utility.h"

#include "resp2_buffer.h"
#include "resp2_types.h"
#include "resp2_string.h"
#include "resp2_error.h"
#include "resp2_number.h"
#include "resp2_bulkstring.h"
#include "resp2_array.h"


typedef struct Resp2_array{
    void** data;
    Resp2_type* type;
    long element_number;
}Resp2_array;

//This function create a new Resp2 array object
Resp2_array* resp2_array_new(long lenght);

void resp2_array_insert(Resp2_array* array, int index, Resp2_type type, void* T);

//This function delete a Resp2 array object
void resp2_array_delete(Resp2_array* array);

Resp2_type resp2_array_get_type(Resp2_array* resp2_array, int index);
void* resp2_array_get_data(Resp2_array* resp2_array, int index);

//This function convert a Resp2 bulkstring to a C string, ready to be sended
int resp2_array_toStr(Resp2_array* array, char** str_to_send);

//This function parse a Resp2 array from a Resp2 buffer
Resp2_array* resp2_array_parser(Resp2_buffer* buffer);