/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 generic type
*/
#pragma once

#include <stdlib.h>

#include "resp2_types.h"
#include "resp2_string.h"
#include "resp2_error.h"
#include "resp2_number.h"
#include "resp2_bulkstring.h"
#include "resp2_array.h"

typedef struct Resp2_generics{
    void* data;
    Resp2_type type;
}Resp2_generics;

//This function create a new Resp2 generics object
Resp2_generics* resp2_generics_new(void* data, Resp2_type type);

//This function delete a Resp2 generics object
void resp2_generics_delete(Resp2_generics* generics);

//This function return the data contained in Resp2 generic object
void* resp2_generics_get_data(Resp2_generics* generics);

//This function return the type contained in Resp2 generic object
Resp2_type resp2_generics_get_type(Resp2_generics* generics);

