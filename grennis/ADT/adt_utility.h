#pragma once

#include <stdio.h>
#include <error.h>

void adt_utils_print_error(const char* file_name, int line, const char* function_name, const char* error_description);