/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 number type "+<value>\r\n"
*/
#include "resp2_number.h"

//This function create a new Resp2 number object
Resp2_number* resp2_number_new(char* data, int lenght){
    Resp2_number* number = malloc(sizeof(Resp2_number));
    number->lenght = lenght;
    number->data = malloc(sizeof(char)*lenght);
    for(int i=0; i<lenght; i++){
        number->data[i] = data[i];
    }
    return number;
}

//This function append a bufferized number to a Resp2 number object
Resp2_number* resp2_number_append(Resp2_number* number, char* data, int lenght){
    if(lenght == 0)
        return number;
    char* new_number = malloc(sizeof(char) * (number->lenght + lenght ));
    for(int i=0; i<number->lenght; i++){
        new_number[i] = number->data[i]; 
    }
    for(int i=0; i<lenght; i++){
        new_number[i+number->lenght] = data[i];
    }
    free(number->data);
    number->data = new_number;
    number->lenght += lenght;

    return number;
}

//This function delete a Resp2 number object
void resp2_number_delete(Resp2_number* number){
    free(number->data);
    free(number);
    return;
}

//This function convert a Resp2 number to a C string, ready to be sended
int resp2_number_toStr(Resp2_number* number, char** str_to_send){
    int total_string_len = 3 + number->lenght;
    char* total_string = malloc(sizeof(char)*total_string_len);
    total_string[0] = ':';
    strncpy(&total_string[1], number->data, number->lenght);
    total_string[total_string_len-2] = '\r';
    total_string[total_string_len-1] = '\n';
    *str_to_send = total_string;
    return total_string_len;
}

//This function parse a Resp2 number from a Resp2 buffer
Resp2_number* resp2_number_parser(Resp2_buffer* buffer){
    int finded_terminator = 0;
    int sign = 0;
    int digit_count = 0;
    int init_pos = buffer->pos;
    int end_pos = init_pos;

    Resp2_number* number = resp2_number_new("", 0);
    while(1){
        if(buffer->pos == buffer->received){
            resp2_number_append(number, &(buffer->buffer[init_pos]), end_pos - init_pos);
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
                        if(digit_count == 0){
                            resp2_number_delete(number);
                            return NULL;
                        }
                        finded_terminator = 1;
                        buffer->pos += 1;
                        break;
    
                    case '\n':
                        resp2_number_delete(number);
                        return NULL;
                        break;

                    case '+': case'-':
                        if(sign == 0 && digit_count == 0){
                                sign += 1;
                                buffer->pos += 1;
                                end_pos += 1;
                            break;
                        }
                        resp2_number_delete(number);
                        return NULL;
                        break;

                    case '0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
                        digit_count+=1;
                        buffer->pos += 1;
                        end_pos += 1;
                        break;
                    
                    default:
                        resp2_number_delete(number);
                        return NULL;

                }
                break;
                

            case 1:
                switch(buffer->buffer[buffer->pos]){
                    case '\n':
                        resp2_number_append(number, &(buffer->buffer[init_pos]), end_pos - init_pos);
                        buffer->pos += 1;
                        return number;
                        break;
                    default:
                        resp2_number_delete(number);
                        return NULL;
                }
                break;
            
            default:
                resp2_number_delete(number);
                return NULL;
        }
    }
    resp2_number_delete(number);
    return NULL;
}

//Convert a resp 2 number to a C long number
long resp2_number_toLong(Resp2_number* number){
    char* endptr = &(number->data[number->lenght]);
    return strtol( number->data, &(endptr) , 10);
}