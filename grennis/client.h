#pragma once

#include <pthread.h>
#include <stdlib.h>

#include "ADT/scl_ts.h"
#include "resp2/resp2.h"

#include "expirer_manager.h"

#define BUFFER_SIZE 1024

typedef struct Client{
    int client_id;
    
    int request_number;
    int respons_number;
    pthread_cond_t send_condition;
    pthread_mutex_t send_mutex;


    int socket_fd;
    int socket_readclosed;

    Expirer_manager* expirer_manager;
    TDBMS* tdbms;

    Resp2_bulkstring* lib_name;
    Resp2_bulkstring* lib_ver;
    pthread_mutex_t connection_variable_mutex;

    pthread_t thread_descriptor;
}Client;


Client* client_new(int id, int socket_fd, TDBMS* tdbms, Expirer_manager* expirer_manager);
void client_delete(Client* client);

void client_send_response(Client* client, int  my_sequence, char* string_to_send, int len);

void client_set_libname(Client* client, Resp2_bulkstring* lib_name);
void client_set_libver(Client* client, Resp2_bulkstring* lib_ver);

int client_cmp_byid(void* e1, void* e2);
int is_ended_client(const void* e1, const void* e2);

#include "command.h"
