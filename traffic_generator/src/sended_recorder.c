#include "sended_recorder.h"

Sended_record* sended_record_new(int node_id, int sequence_number){
    Sended_record* new_record = malloc(sizeof(Sended_record));
    new_record->node_id = node_id;
    new_record->last_sequence = sequence_number;
    return new_record;
}

Sended_record* sended_record_update(Sended_record* record, int sequence_number){
    record->last_sequence = sequence_number;
    return record;
}

void sended_record_destroyer(Sended_record* record){
    free(record);
    return;
}

void print_T(void* T){
    Sended_record* record = (Sended_record*) T;
    printf("Id:%d-Seq:%d", record->node_id, record->last_sequence);
    return;
}

int sended_record_compar(const void* e1, const void* e2){
    if( ((Sended_record*)e1)->node_id > ((Sended_record*)e2)->node_id )
        return 1;
    if( ((Sended_record*)e1)->node_id < ((Sended_record*)e2)->node_id )
        return -1;
    return 0;
}