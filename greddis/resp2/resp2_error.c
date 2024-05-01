/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 error type "-<value>\r\n"
*/
#include "resp2_error.h"

//This function create a new Resp2 error object
Resp2_error* resp2_error_new(const char* error_type, char* message, int message_len){
    Resp2_error* resp2_error = malloc(sizeof(Resp2_error));
    resp2_error->data = malloc(sizeof(char) * (message_len + strlen(error_type)) );
    resp2_error->lenght = message_len + strlen(error_type);
    strncpy(resp2_error->data, error_type, strlen(error_type));
    strncpy(&resp2_error->data[strlen(error_type)], message, message_len);
    return resp2_error;
}


//This function append a bufferized error to a Resp2 error object
Resp2_error* resp2_error_append(Resp2_error* error, char* data, int lenght){
    if(lenght == 0)
        return error;
    char* new_error = malloc(sizeof(char) * (error->lenght + lenght ));
    for(int i=0; i<error->lenght; i++){
        new_error[i] = error->data[i]; 
    }
    for(int i=0; i<lenght; i++){
        new_error[i+error->lenght] = data[i];
    }
    free(error->data);
    error->data = new_error;
    error->lenght += lenght;

    return error;
}

//This function delete a Resp2 error object
void resp2_error_delete(Resp2_error* error){
    free(error->data);
    free(error);
    return;
}

//This function convert a Resp2 error to a C string, ready to be sended
int resp2_error_toStr(Resp2_error* error, char** str_to_send){
    int total_string_len = 3 + error->lenght;
    char* total_string = malloc(sizeof(char)*total_string_len);
    total_string[0] = '-';
    strncpy(&total_string[1], error->data, error->lenght);
    total_string[total_string_len-2] = '\r';
    total_string[total_string_len-1] = '\n';
    *str_to_send = total_string;
    return total_string_len;
}

//This function parse a Resp2 error from a Resp2 buffer
Resp2_error* resp2_error_parser(Resp2_buffer* buffer){
    int finded_terminator = 0;
    int init_pos = buffer->pos;
    int end_pos = init_pos;

    Resp2_error* error = resp2_error_new("", "", 0);
    while(1){
        if(buffer->pos == buffer->received){
            resp2_error_append(error, &(buffer->buffer[init_pos]), end_pos - init_pos);
            init_pos = 0;
            end_pos = 0;
            buffer->received = resp2_buffer_receive(buffer);
            if(buffer->received <= 0)
                return NULL;
        }

        switch(finded_terminator){
            case 0:
                switch (buffer->buffer[buffer->pos]){
                    case '\r':
                        finded_terminator = 1;
                        buffer->pos += 1;
                        break;
    
                    case '\n':
                        resp2_error_delete(error);
                        return NULL;
                        break;

                    default:
                        buffer->pos += 1;
                        end_pos += 1;
                        break;
                }
                break;

            case 1:
                switch(buffer->buffer[buffer->pos]){
                    case '\n':
                        resp2_error_append(error, &(buffer->buffer[init_pos]), end_pos - init_pos);
                        buffer->pos += 1;
                        return error;
                        break;
                    default:
                        resp2_error_delete(error);
                        return NULL;
                }
                break;
            
            default:
                resp2_error_delete(error);
                return NULL;
        }
    }
    resp2_error_delete(error);
    return NULL;
}
