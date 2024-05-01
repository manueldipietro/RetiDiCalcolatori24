/**
 * Author: Manuel Di Pietro
 * Date: 9-04-2024
 * Object: In-memory T-DBMS v.2
 * Version: 2.0
*/
#include "tdbms.h"

/**
 *  Macro that defines the block of code that manages incoming
 *  concurrency for read-only access to the data structure
*/
#define READ_BLK(list, return_on_error){\
    int ret;\
    \
    ret = sem_wait(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock write resource");\
        return return_on_error;\
    }\
    \
    ret = sem_wait(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock read resource");\
        return return_on_error;\
    }\
    \
    list->reader_number += 1;\
    \
    ret = sem_post(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
    \
    ret = sem_post(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
    \
}

/**
 *  Macro that defines the block of code that manages outgoing
 *  concurrency for read-only access to the data structure
*/
#define READ_RLS(list, return_on_error){\
    int ret = sem_wait(&(list->reader_sem));\
    \
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock read resource");\
        return return_on_error;\
    }\
    \
    list->reader_number -= 1;\
    \
    ret = sem_post(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
}

/**
 *  Macro that defines the block of code that manages incoming
 *  concurrency for write access to the data structure
*/
#define WRITE_BLK(list, return_on_error){\
    int ret;\
    \
    ret = sem_wait(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock write resource");\
        return return_on_error;\
    }\
    \
    int exit = 0;\
    while(!exit){\
        ret = sem_wait(&(list->reader_sem));\
        if(ret < 0){\
            adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to lock read resource");\
            return return_on_error;\
        }\
        \
        if(list->reader_number == 0)\
            exit = 1;\
        \
        ret = sem_post(&(list->reader_sem));\
        if(ret < 0){\
            adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
            return return_on_error;\
        }\
        \
    }\
}

/**
 *  Macro that defines the block of code that manages outcoming
 *  concurrency for write access to the data structure
*/
#define WRITE_RLS(list, return_on_error){\
    int ret;\
    \
    ret = sem_post(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
}

//This function create a new TDBMS object
TDBMS* tdbms_new(){
    TDBMS* db = malloc(sizeof(TDBMS));
    db->tree = avl_new();
    sem_init(&(db->reader_sem), 0, 1);
        //TO-DO gestione errore
    sem_init(&(db->writer_sem), 0, 1);
        //TO-DO gestione errore
    return db;
}

//This function insert a new record into TDBMS
int tdbms_set(TDBMS* db, Resp2_bulkstring* key, Resp2_bulkstring* value, Tdbms_insert_mode mode, Resp2_bulkstring** old_value){
    *old_value = NULL;
    int writed = 0;

    WRITE_BLK(db, -1);
    
    if(mode == TDBMS_ONLY_EXISTS){
        //Redis call this XX
        Tdbms_record* record = tdbms_record_new(key, value);
        Tdbms_record* finded = avl_research(db->tree, record, tdbms_record_cmp);
        if(finded != NULL){
            *old_value = resp2_bulkstring_new(finded->value->data, finded->value->lenght);
            tdbms_record_update(finded, value);
            writed = 1;
        }
        tdbms_record_delete(record);
        WRITE_RLS(db, -1);
        return writed;
    }
    
    if(mode == TDBMS_ONLY_NOT_EXISTS){
        //Redis call this NX
        writed = 1;
        Tdbms_record* record = tdbms_record_new(key, value);
        Tdbms_record* finded = avl_insert(db->tree, record, tdbms_record_cmp);
        if(finded != NULL){
            *old_value = resp2_bulkstring_new(finded->value->data, finded->value->lenght);
            tdbms_record_delete(record);
            writed = 0;
        }
        WRITE_RLS(db, -1);
        return writed;
    }

    if(mode == TDBMS_OVERWRITE){
        //Reddis default case
        Tdbms_record* record = tdbms_record_new(key, value);
        Tdbms_record* finded = avl_insert(db->tree, record, tdbms_record_cmp);
        if(finded != NULL){
            *old_value = resp2_bulkstring_new(finded->value->data, finded->value->lenght);
            tdbms_record_update(record, value);
            tdbms_record_delete(record);
        }
        writed = 1;
        WRITE_RLS(db, -1);
        return writed;
    }

    adt_utils_print_error(__FILE__, __LINE__, __func__, "Unrecognized write mode!");
    WRITE_RLS(db, -1);
    return -1;
}

//This function remove a node from TDBMS
void tdbms_remove(TDBMS* db, Resp2_bulkstring* key){
    WRITE_BLK(db, );
    Tdbms_record* record = tdbms_record_new(key, NULL);
    Tdbms_record* removed = avl_remove(db->tree, record, tdbms_record_cmp);
    WRITE_RLS(db,);
    tdbms_record_delete(record);
    tdbms_record_delete(removed);
}

//This function research a node from TDBMS (return the value)
Resp2_bulkstring* tdbms_get(TDBMS* db, Resp2_bulkstring* key){
    Tdbms_record* record = tdbms_record_new(key, NULL);
    READ_BLK(db, NULL);
    Tdbms_record* finded = avl_research(db->tree, record, tdbms_record_cmp);
    READ_RLS(db, NULL);
    
    tdbms_record_delete(record);

    if(finded == NULL)
        return resp2_bulkstring_new(NULL, -1);
    
    return resp2_bulkstring_new(finded->value->data, finded->value->lenght);
}


//This function delete a TDBMS object
void tdbms_delete(TDBMS* db){
    avl_delete_andfreeinfo(db->tree, tdbms_record_delete);
    sem_destroy(&(db->reader_sem));
        //TODO Gestione errore
    sem_destroy(&(db->writer_sem));
        //TODO Gestione errore
    free(db);
    return;
}

