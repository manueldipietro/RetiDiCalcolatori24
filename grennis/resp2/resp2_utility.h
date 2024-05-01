/**
 * Author: Manuel Di Pietro
 * Date: 06-04-2024
 * Object: Resp 2 useful functions
*/

#pragma once

#include <stdio.h>
#include <error.h>

//This function prints an error message, indicating the file, line, function and contents of the errno variable
void resp2_print_error(const char* file_name, int line, const char* function_name, const char* error_description);

//This function count the number of digit used for rappresentate a decimal number
int resp2_send_count_number_digit(int number);