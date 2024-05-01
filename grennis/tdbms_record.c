#include "tdbms_record.h"

Tdbms_record* tdbms_record_new(Resp2_bulkstring* key, Resp2_bulkstring* value){
    Tdbms_record* record = malloc(sizeof(Tdbms_record));
    record->key = resp2_bulkstring_new(key->data, key->lenght);
    record->value = resp2_bulkstring_new(key->data, key->lenght);
    return record;
}

int tdbms_record_cmp(const void* _e1, const void* _e2){
    Tdbms_record* e1 = (Tdbms_record*) _e1;
    Tdbms_record* e2 = (Tdbms_record*) _e2;
    return resp2_bulkstring_cmp(e1->key, e2->key);
}

void tdbms_record_delete(void* record){
    Tdbms_record* record_casted = (Tdbms_record*) record;
    if(record_casted->key != NULL) resp2_bulkstring_delete(record_casted->key);
    if(record_casted->key != NULL) resp2_bulkstring_delete(record_casted->value);
    free(record_casted);
    return;
}

void tdbms_record_update(Tdbms_record* record, Resp2_bulkstring* new_value){
    record->value = new_value;
    return;
}