/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 parser
*/
#include "resp2_parser.h"


Resp2_generics* resp2_parser(Resp2_buffer* buffer){
    long ret = 0;
    
    while(buffer->pos == buffer->received){
        ret = resp2_buffer_receive(buffer);
        if(ret == -1){
            return NULL;
        }
    }
    switch (buffer->buffer[buffer->pos]){
        case '+':
            buffer->pos += 1;
            Resp2_string* string = resp2_string_parser(buffer);
            if(string == NULL)
                return NULL;
            return resp2_generics_new(string, RESP2_STRING);
            break;

        case '-':
            buffer->pos += 1;
            Resp2_error* error = resp2_error_parser(buffer);
            if(error == NULL)
                return NULL;
            return resp2_generics_new(error, RESP2_ERROR);
            break;

        case ':':
            buffer->pos += 1;
            Resp2_number* number = resp2_number_parser(buffer);
            if(number == NULL)
                return NULL;
            return resp2_generics_new(number, RESP2_NUMBER);
            return NULL;
            break;
        
        case '$':
            buffer->pos += 1;
            Resp2_bulkstring* bulkstring = resp2_bulkstring_parser(buffer);
            if(bulkstring == NULL){
                return NULL;
            }
            return resp2_generics_new(bulkstring, RESP2_BULKSTRING);
            break;
        
        case '*':
            buffer->pos += 1;
            Resp2_array* array = resp2_array_parser(buffer);
            if(array == NULL)
                return NULL;
            return resp2_generics_new(array, RESP2_ARRAY);
            break;
        default:
            buffer->pos += 1;
            return NULL;
            break;
    }
    return NULL;
}
