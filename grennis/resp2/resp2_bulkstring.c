/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "$<len>\r\n<value>\r\n"
*/
#include "resp2_bulkstring.h"


//This function create a new Resp2 bulkstring object
Resp2_bulkstring* resp2_bulkstring_new(char* data, long lenght){
    Resp2_bulkstring* bulkstring = malloc(sizeof(Resp2_bulkstring));
    bulkstring->lenght = lenght;
    
    if(lenght == -1){
        bulkstring->data = NULL;
        return bulkstring;
    }

    bulkstring->data = malloc(sizeof(char)*lenght);
    for(int i=0; i<lenght; i++){
        bulkstring->data[i] = data[i];
    }
    return bulkstring;
}

//This function delete a Resp2 bulkstring object
void resp2_bulkstring_delete(Resp2_bulkstring* bulkstring){
    free(bulkstring->data);
    free(bulkstring);
    return;
}

//This function convert a Resp2 bulkstring to a C string, ready to be sended
int resp2_bulkstring_toStr(Resp2_bulkstring* bulkstring, char** str_to_send){
    //NULL STRING CASE 
    if(bulkstring->lenght == -1){
        char* total_string = malloc(sizeof(char)*5);
        strncpy(total_string, "$-1\r\n", 5);
        *str_to_send = total_string;
        return 5;
    }

    int digits_len =  resp2_send_count_number_digit(bulkstring->lenght);
    char* digits = malloc(sizeof(char)*(digits_len+1)); 
    snprintf(digits, digits_len+1, "%d", bulkstring->lenght);

    int total_string_len = 5 + bulkstring->lenght + digits_len;    
    char* total_string = malloc(sizeof(char)*total_string_len);
    
    total_string[0] = '$';
    strncpy(&total_string[1], digits, digits_len);
    total_string[1+digits_len] = '\r';
    total_string[2+digits_len] = '\n';
    strncpy(&total_string[3+digits_len], bulkstring->data, bulkstring->lenght);
    total_string[3+digits_len+bulkstring->lenght] = '\r';
    total_string[4+digits_len+bulkstring->lenght] = '\n';

    *str_to_send = total_string;
    free(digits);
    return total_string_len;
}

//This function parse a Resp2 bulkstring from a Resp2 buffer
Resp2_bulkstring* resp2_bulkstring_parser(Resp2_buffer* buffer){
    Resp2_number* number = resp2_number_parser(buffer);
    if(number == NULL)
        return NULL;
    long n = resp2_number_toLong(number);
    free(number);
    if(n==-1){
        return resp2_bulkstring_new(NULL, -1);
    }
    
    if(n < -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Bulkstring: unvalid lenght!");
        return NULL;
    }

    char* parsed_string = malloc(sizeof(char)*n);
    for(long i=0; i<n; i++){
        if(buffer->pos == buffer->received){
            buffer->received = resp2_buffer_receive(buffer);
            if(buffer->received <= 0)
                return NULL;
        }
        parsed_string[i] = buffer->buffer[buffer->pos];
        buffer->pos += 1;
    }

    if(buffer->pos == buffer->received){
        buffer->received = resp2_buffer_receive(buffer);
        if(buffer->received <= 0)
            return NULL;
    }
    if(buffer->buffer[buffer->pos] != '\r'){
        resp2_print_error(__FILE__, __LINE__, __func__, "Unvalid string");
        buffer->pos +=1 ;
        return NULL;
    }
    buffer->pos +=1 ;

    if(buffer->pos == buffer->received){
        buffer->received = resp2_buffer_receive(buffer);
        if(buffer->received <= 0)
            return NULL;
    }
    if(buffer->buffer[buffer->pos] != '\n'){
        resp2_print_error(__FILE__, __LINE__, __func__, "Unvalid string");
        buffer->pos +=1 ;
        return NULL;
    }
    buffer->pos +=1 ;

    Resp2_bulkstring* bulkstring = resp2_bulkstring_new(parsed_string, n);
    free(parsed_string);
    return bulkstring;
}

//This function compare two Resp2 bulkstring
int resp2_bulkstring_cmp(const void* _e1, const void* _e2){
    Resp2_bulkstring* e1 = (Resp2_bulkstring*) _e1;
    Resp2_bulkstring* e2 = (Resp2_bulkstring*) _e2;
    
    int min_len = e1->lenght < e2->lenght ? e1->lenght : e2->lenght;

    int ret = strncmp(e1->data, e2->data, min_len);

    if(ret == 0 && e1->lenght == e2->lenght)
        return 0;
    if(ret == 0)
        return (e1->lenght > e2->lenght ? 1 : -1);
    return ret;
}