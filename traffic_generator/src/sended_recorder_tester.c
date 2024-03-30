#include "scl.h"

typedef struct Sended_record{
    int node_id;
    int last_sequence;
}Sended_record;

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

int broadcaster_is_sendable(Linked_list* list, int node_id, int sequence){
    Sended_record ra = {node_id, sequence};

    Sended_record* finded = (Sended_record* ) linked_list_search_by_value(list, &ra, sended_record_compar);
    
    if(finded==NULL){
        Sended_record* new_record = sended_record_new(node_id, sequence);
        linked_list_add(list, new_record);
        return 1;
    }
    if( finded->last_sequence < sequence ){
        sended_record_update(finded, sequence);
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]){
    Sended_record r1 = {0, 1};
    Sended_record r2 = {0, 2};
    Sended_record r3 = {1, 1};
    Sended_record r4 = {1, 2};
    Sended_record r5 = {2, 0};

    Linked_list* test_list = linked_list_new();
    printSCL(test_list, print_T);
    
    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 0, 2));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 0, 3));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 0, 2));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 0, 3));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 1, 2));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 3, 3));
    printSCL(test_list, print_T);

    printf("---------------------------------\n");
    printf("IsSendable: %d\n", broadcaster_is_sendable(test_list, 1, 2));
    printSCL(test_list, print_T);



    return 0;
}

