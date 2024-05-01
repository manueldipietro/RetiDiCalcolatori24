#pragma once

#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#include "ADT/scl_ordered_ts.h"
#include "resp2/resp2.h"
#include "tdbms.h"
#include "timespec.h"
#include "utility.h"

typedef struct Expirer_manager_record{
    Resp2_bulkstring* key;
    struct timespec timestamp;
}Expirer_manager_record;

Expirer_manager_record* expirer_manager_record_new(Resp2_bulkstring* key, struct timespec timestamp);
int expirer_manager_record_cmp(const void* e1, const void* e2);
int expirer_manager_timestamp_cmp(const void* e1, const void* e2);
void expirer_manager_record_delete(void* record);

typedef struct Expirer_manager{
    Scl_ordered* queue;
    TDBMS* tdbms;

    pthread_cond_t sync_condition;
    pthread_mutex_t sync_mutex;

    pthread_t thread_descriptor;
}Expirer_manager;

Expirer_manager* expirer_manager_new(TDBMS* tdbms);
//-PRIVATE: void* expirer_manager_process(void* args);
void expirer_manager_append(Expirer_manager* expirer_manager, Resp2_bulkstring* key, struct timespec timestamp);
void expirer_manager_remove(Expirer_manager* expirer_manager, Resp2_bulkstring* key);