/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "+<value>\r\n"
*/
#include "resp2_string.h"

//This function create a new Resp2 string object
Resp2_string* resp2_string_new(char* data, int lenght){
    Resp2_string* string = malloc(sizeof(Resp2_string));
    string->lenght = lenght;
    string->data = malloc(sizeof(char)*lenght);
    for(int i=0; i<lenght; i++){
        string->data[i] = data[i];
    }
    return string;
}

//This function append a bufferized string to a Resp2 string object
Resp2_string* resp2_string_append(Resp2_string* string, char* data, int lenght){
    if(lenght == 0)
        return string;
    char* new_string = malloc(sizeof(char) * (string->lenght + lenght ));
    for(int i=0; i<string->lenght; i++){
        new_string[i] = string->data[i]; 
    }
    for(int i=0; i<lenght; i++){
        new_string[i+string->lenght] = data[i];
    }
    free(string->data);
    string->data = new_string;
    string->lenght += lenght;

    return string;
}

//This function delete a Resp2 string object
void resp2_string_delete(Resp2_string* string){
    free(string->data);
    free(string);
    return;
}

//This function convert a Resp2 string to a C string, ready to be sended
int resp2_string_toStr(Resp2_string* string, char** str_to_send){
    int total_string_len = 3 + string->lenght;
    char* total_string = malloc(sizeof(char)*total_string_len);
    total_string[0] = '+';
    strncpy(&total_string[1], string->data, string->lenght);
    total_string[total_string_len-2] = '\r';
    total_string[total_string_len-1] = '\n';
    *str_to_send = total_string;
    return total_string_len;
}

//This function parse a Resp2 string from a Resp2 buffer
Resp2_string* resp2_string_parser(Resp2_buffer* buffer){
    int finded_terminator = 0;
    int init_pos = buffer->pos;
    int end_pos = init_pos;

    Resp2_string* string = resp2_string_new("", 0);
    while(1){
        if(buffer->pos == buffer->received){
            resp2_string_append(string, &(buffer->buffer[init_pos]), end_pos - init_pos);
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
                        resp2_string_delete(string);
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
                        resp2_string_append(string, &(buffer->buffer[init_pos]), end_pos - init_pos);
                        buffer->pos += 1;
                        return string;
                        break;
                    default:
                        resp2_string_delete(string);
                        return NULL;
                }
                break;
            
            default:
                resp2_string_delete(string);
                return NULL;
        }
    }
    resp2_string_delete(string);
    return NULL;
}
