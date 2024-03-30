#pragma once

#include <stdio.h>
#include <errno.h>
#include <time.h>

void print_error(const char* file_name, int line, const char* function_name, const char* error_description);
int msleep(long msec);
