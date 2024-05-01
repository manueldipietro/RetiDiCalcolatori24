/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object:
*/
#include "utility.h"

//This function prints an error message, indicating the file, line, function and contents of the errno variable
void print_error(const char* file_name, int line, const char* function_name, const char* error_description){
    printf("In function %s (file: %s - line: %d):\n", function_name, file_name, line);
    printf("\texecution error: %s\n", error_description);
    perror("\terrno");
    return;
}