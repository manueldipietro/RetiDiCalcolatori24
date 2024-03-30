#include "scl.h"

typedef struct Sended_record{
    int node_id;
    int last_sequence;
}Sended_record;

Sended_record* sended_record_new(int node_id, int sequence_number);
Sended_record* sended_record_update(Sended_record* record, int sequence_number);
void sended_record_destroyer(Sended_record* record);
int sended_record_compar(const void* e1, const void* e2);
void print_T(void* T);