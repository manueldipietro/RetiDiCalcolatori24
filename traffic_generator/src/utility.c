#include "utility.h"

void print_error(const char* file_name, int line, const char* function_name, const char* error_description){
    printf("In function %s (file: %s - line: %d):\n", function_name, file_name, line);
    printf("\texecution error: %s\n", error_description);
    perror("\terrno: ");
}

int msleep(long msec){
    struct timespec ts;
    int res;
    if (msec < 0){
        errno = EINVAL;
        return -1;
    }
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    do{
        res = nanosleep(&ts, &ts);
    }while (res && errno == EINTR);
    return res;
}