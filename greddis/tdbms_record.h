#pragma once

#include <stdlib.h>
#include <semaphore.h>

#include "resp2/resp2.h"

typedef struct Tdbms_record{
    Resp2_bulkstring* key;
    Resp2_bulkstring* value;
}Tdbms_record;

Tdbms_record* tdbms_record_new(Resp2_bulkstring* key, Resp2_bulkstring* value);

void tdbms_record_update(Tdbms_record* record, Resp2_bulkstring* new_value);

void tdbms_record_delete(void* record);

int tdbms_record_cmp(const void* _e1, const void* _e2);