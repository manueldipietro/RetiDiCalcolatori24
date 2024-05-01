/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: resp 2 string type "*<len>\r\n<element-1>\r\n<elemenet-2>\r\n..."
*/
#include "resp2_array.h"

//This function create a new Resp2 array object
Resp2_array* resp2_array_new(long lenght){
    Resp2_array* array = malloc(sizeof(Resp2_array));
    
    //Null case
    if(lenght == -1){
        array->element_number = -1;
        array->data = NULL;
        array->type = NULL;
        return NULL;
    }

    array->data = malloc(sizeof(void*)*lenght);
    array->type = malloc(sizeof(Resp2_type)*lenght);
    array->element_number = lenght;

    //Initialize array with null string
    for(int i=0; i< array->element_number; i++){
        array->type[i] = RESP2_BULKSTRING;
        array->data[i] = resp2_bulkstring_new(NULL, -1);
    }

    return array;
}

//This function delete a Resp2 array object
void resp2_array_delete(Resp2_array* array){
    if(array->element_number != -1 || array->data == NULL){
        free(array);
        return;
    }

    for(long i=0; i<array->element_number; i++){
        switch(resp2_array_get_type(array, i)){
            case RESP2_STRING:
                resp2_string_delete(resp2_array_get_data(array, i));
                break;

            case RESP2_ERROR:
                resp2_error_delete(resp2_array_get_data(array, i));
                break;

            case RESP2_NUMBER:
                resp2_number_delete(resp2_array_get_data(array, i));
                break;

            case RESP2_BULKSTRING:
                resp2_bulkstring_delete(resp2_array_get_data(array, i));
                break;

            case RESP2_ARRAY:
                resp2_array_delete(resp2_array_get_data(array, i));
                break;

            default:
                resp2_print_error(__FILE__, __LINE__, __func__, "Unrecognized type!");
                return;
                break;
        }
    }

    free(array);
    return;
}


Resp2_type resp2_array_get_type(Resp2_array* array, int index){
    if(index > array->element_number)
        return -1;
    return array->type[index];
}

void* resp2_array_get_data(Resp2_array* array, int index){
    if(index > array->element_number)
        return NULL;
    return array->data[index];
}

void resp2_array_insert(Resp2_array* array, int index, Resp2_type type, void* T){
    if(index > array->element_number){
        resp2_print_error(__FILE__, __LINE__, __func__, "Invalid arguments!");
        return;
    }
    array->data[index] = T;
    array->type[index] = type;
    return;
}


//This function convert a Resp2 array to a C string, ready to be sended
int resp2_array_toStr(Resp2_array* array, char** str){
    //null case
    if(array->element_number == -1){
        char* total_string = malloc(sizeof(char)*5);
        strncpy(total_string, "*-1\r\n", 5);
        *str = total_string;
        return 5;
    }

    int digits_len =  resp2_send_count_number_digit(array->element_number);
    char* digits = malloc(sizeof(char)*(digits_len+1)); 
    snprintf(digits, digits_len+1, "%ld", array->element_number);

    int total_string_len = 3 + digits_len;    
    char* total_string = malloc(sizeof(char)*total_string_len);
    
    total_string[0] = '*';
    strncpy(&(total_string[1]), digits, digits_len);
    total_string[1+digits_len] = '\r';
    total_string[2+digits_len] = '\n';

    free(digits);


    for(int i=0; i<array->element_number; i++){
        char* partial_string;
        int partial_string_len;

        switch (resp2_array_get_type(array, i)){
            case RESP2_STRING:
                partial_string_len = resp2_string_toStr( (Resp2_string*) resp2_array_get_data(array, i), &partial_string);
            break;
            case RESP2_ERROR:
                partial_string_len = resp2_error_toStr( (Resp2_error*) resp2_array_get_data(array, i), &partial_string);
            break;
            case RESP2_NUMBER:
                partial_string_len = resp2_number_toStr( (Resp2_number*) resp2_array_get_data(array, i), &partial_string);
            break;
            case RESP2_BULKSTRING:
                partial_string_len = resp2_bulkstring_toStr( (Resp2_bulkstring*) resp2_array_get_data(array, i), &partial_string);
            break;
            case RESP2_ARRAY:
                partial_string_len = resp2_array_toStr( (Resp2_array*) resp2_array_get_data(array, i), &partial_string);    
            break;        
            default:
                resp2_print_error(__FILE__, __LINE__, __func__, "Unble to recognize element type");
                return -1;
            break;
        }
        char* new_total_string = malloc(sizeof(char)*(total_string_len + partial_string_len));
        strncpy(new_total_string, total_string, total_string_len);
        strncpy(&(new_total_string[total_string_len]), partial_string, partial_string_len);
        total_string_len += partial_string_len;

        free(total_string);
        total_string = new_total_string;
    }

    *str = total_string;

    return total_string_len;
}

//This function parse a Resp2 array from a Resp2 buffer
Resp2_array* resp2_array_parser(Resp2_buffer* buffer){
    Resp2_number* number = resp2_number_parser(buffer);
    if(number == NULL)
        return NULL;
    long n = resp2_number_toLong(number);
    free(number);
    if(n==-1){
        return resp2_array_new(-1);
    }
    if(n < -1){
        resp2_print_error(__FILE__, __LINE__, __func__, "Unvalid lenght!");
        return NULL;
    }

    Resp2_array* array = resp2_array_new(n);
    for(long i=0; i<n; i++){
        if(buffer->pos == buffer->received){
            buffer->received = resp2_buffer_receive(buffer);
            if(buffer->received <= 0)
                return NULL;
        }

        //Qui è necessario implementare che se esiste già un tipo si deve distruggere!!-->Su insert
        switch(buffer->buffer[buffer->pos]){
            case '+':
                buffer->pos += 1;
                resp2_array_insert(array, i, RESP2_STRING, (void*) resp2_string_parser(buffer));
                break;
            
            case '-':
                buffer->pos += 1;
                resp2_array_insert(array, i, RESP2_ERROR, (void*) resp2_error_parser(buffer));
                break;
            
            case ':':
                buffer->pos += 1;
                resp2_array_insert(array, i, RESP2_NUMBER, (void*) resp2_number_parser(buffer));
                break;
            
            case '$':
                buffer->pos += 1;
                resp2_array_insert(array, i, RESP2_BULKSTRING, (void*) resp2_bulkstring_parser(buffer));
                break;
            
            case '*':
                buffer->pos +=1;
                resp2_array_insert(array, i, RESP2_ARRAY, (void*) resp2_array_parser(buffer));
                break;
            
            default:
                buffer->pos += 1;
                resp2_print_error(__FILE__, __LINE__, __func__, "Unrecognized type!");
                resp2_array_delete(array);
                return NULL;
                break;
        }
    }

    return array;
}
