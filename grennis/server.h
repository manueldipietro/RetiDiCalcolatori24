#pragma once

#include <pthread.h>
#include <stdlib.h>

#include "utility.h"
#include "expirer_manager.h"
#include "tdbms.h"
#include "client.h"

typedef struct Server{
    int server_port;
    int maximum_connection_number;
    int last_connection_id;
    int socket_fd;
    SclTS* clients;
    TDBMS* tdbms;
    Expirer_manager* expirer_manager;
}Server;

void server_new(int port, int max_con, TDBMS* tdbms, Expirer_manager* expirer_manager);