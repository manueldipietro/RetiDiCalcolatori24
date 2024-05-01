#include "adt_utility.h"

/**
 * This function print a message after an error, including the file_name, line and the function where the error happens
 * Print also the errno variable;
*/
void adt_utils_print_error(const char* file_name, int line, const char* function_name, const char* error_description){
    printf("In function %s (file: %s - line: %d):\n", function_name, file_name, line);
    printf("\texecution error: %s\n", error_description);
    perror("\terrno");
}
