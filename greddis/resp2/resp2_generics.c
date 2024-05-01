/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "+<value>\r\n"
*/
#include "resp2_generics.h"

//This function create a new Resp2 generics object
Resp2_generics* resp2_generics_new(void* data, Resp2_type type){
    Resp2_generics* generics = malloc(sizeof(Resp2_generics));
    generics->data = data;
    generics->type = type;
    return generics;
}


//This function delete a Resp2 generics object
void resp2_generics_delete(Resp2_generics* generics){
    if(generics == NULL)
        return;
    if(generics->data!=NULL){
        switch(generics->type){
            case RESP2_STRING:
                resp2_string_delete(generics->data);
                break;
        
            case RESP2_ERROR:
                resp2_error_delete(generics->data);
                break;

            case RESP2_NUMBER:
                resp2_number_delete(generics->data);
                break;

            case RESP2_BULKSTRING:
                resp2_bulkstring_delete(generics->data);
                break;

            case RESP2_ARRAY:
                resp2_array_delete(generics->data);
                break;
        }
    }
    free(generics);
    return;
}

//This function return the data contained in Resp2 generic object
void* resp2_generics_get_data(Resp2_generics* generics){
    return generics->data;
}

//This function return the type contained in Resp2 generic object
Resp2_type resp2_generics_get_type(Resp2_generics* generics){
    return generics->type;
}