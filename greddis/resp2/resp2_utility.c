/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 useful functions
*/
#include "resp2_utility.h"

//This function prints an error message, indicating the file, line, function and contents of the errno variable
void resp2_print_error(const char* file_name, int line, const char* function_name, const char* error_description){
    printf("In function %s (file: %s - line: %d):\n", function_name, file_name, line);
    printf("\texecution error: %s\n", error_description);
    perror("\terrno");
    return;
}

int resp2_send_count_number_digit(int number){
    int digit = 0;
    //if(number < 0) digit++;
    do{
        number = number/10;
        digit++;
    }while(number != 0);
    return digit;
}