#pragma once

#include <stdio.h>
#include <error.h>
#include <time.h>
#include <unistd.h>

void print_error(const char* file_name, int line, const char* function_name, const char* error_description);