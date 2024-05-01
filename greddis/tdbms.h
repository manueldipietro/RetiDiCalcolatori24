/**
 * Author: Manuel Di Pietro
 * Date: 9-04-2024
 * Object: In-memory T-DBMS v.2
 * Version: 2.0
*/
#pragma once

#include <stdlib.h>

#include "ADT/avl.h"
#include "ADT/adt_utility.h"

#include "resp2/resp2.h"
#include "tdbms_record.h"

#include <semaphore.h>

//TDBMS definition
typedef struct TDBMS{
    Avl* tree;
    int reader_number;
    sem_t reader_sem;
    sem_t writer_sem;
}TDBMS;

//This enum defines the possible insert_mode
typedef enum Tdbms_insert_mode{
    TDBMS_ONLY_NOT_EXISTS, TDBMS_ONLY_EXISTS, TDBMS_OVERWRITE
}Tdbms_insert_mode;

//This function create a new TDBMS object
TDBMS* tdbms_new();

//This function insert a new record into TDBMS
int tdbms_set(TDBMS* db, Resp2_bulkstring* key, Resp2_bulkstring* value, Tdbms_insert_mode mode, Resp2_bulkstring** old_value);

//This function remove a node from TDBMS
void tdbms_remove(TDBMS* db, Resp2_bulkstring* key);

//This function research a node from TDBMS (return the value)
Resp2_bulkstring* tdbms_get(TDBMS* db, Resp2_bulkstring* key);

//This function delete a TDBMS object
void tdbms_delete(TDBMS* db);

