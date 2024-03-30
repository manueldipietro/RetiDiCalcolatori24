#include "traffic_analyzer.h"

static int work = 0;

/**
 * 
*/
#include "sended_recorder.h"

Throughput_record* throughput_record_new(int node_id, int is_odd){
    Throughput_record* new_record = malloc(sizeof(Throughput_record));
    new_record->node_id = node_id;
    new_record->second_odd = (is_odd ? 1 : 0);
    new_record->second_even = (is_odd ? 0 : 1);
    return new_record;
}

Throughput_record* throughput_record_update(Throughput_record* record, int is_odd){
    if(is_odd) record->second_odd += 1;
    if(!is_odd) record->second_even += 1;
    return record;
}

void throughput_record_destroyer(Throughput_record* record){
    free(record);
    return;
}

void throughput_record_print_T(void* T){
    Throughput_record* record = (Throughput_record*) T;
    printf(" %d: %d - %d ", record->node_id, record->second_even, record->second_odd);
    return;
}

int throughput_record_compar(const void* e1, const void* e2){
    if( ((Throughput_record*)e1)->node_id > ((Throughput_record*)e2)->node_id )
        return 1;
    if( ((Throughput_record*)e1)->node_id < ((Throughput_record*)e2)->node_id )
        return -1;
    return 0;
}

void throughput_record_zeroize_aux(Linked_list_node* node, int is_odd){
    if(node == NULL)
        return;
    Throughput_record* record = (Throughput_record*) node->T;
    if(is_odd){
        record->second_odd = 0;
    }else{
        record->second_even = 0;
    }
    throughput_record_zeroize_aux(node->next, is_odd);
}

void throughput_record_zeroize(Linked_list* list, int is_odd){
    throughput_record_zeroize_aux(list->first_element, is_odd);
    return;
}

void traffic_analyzer_updatelist(Linked_list* list, int node_id, int is_odd){
    Throughput_record to_search = {node_id, 0, 0};
    Throughput_record* finded = linked_list_search_by_value(list, (void*) &to_search, throughput_record_compar);
    if(finded == NULL){
        Throughput_record* to_add = throughput_record_new(node_id, is_odd);
        linked_list_add(list, (void*) to_add);
        return;
    }
    throughput_record_update(finded, is_odd);
    return;    
}


void traffic_analyzer_print_troughput_aux(Linked_list_node* node){
    if(node==NULL)
        return;
    printf("%d: %03d, ", ((Throughput_record*)node->T)->node_id, ((Throughput_record*)node->T)->second_even + ((Throughput_record*)node->T)->second_odd );
    traffic_analyzer_print_troughput_aux(node->next);    
}

void traffic_analyzer_print_troughput(Linked_list* list){
    printf("Throughput: ");
    traffic_analyzer_print_troughput_aux(list->first_element);
    printf("\n");
}


void traffic_analyzer(int pipe_broadcaster_to_traffanalz_fd[2]){ 
    int ret;
    
    ret = prctl(PR_SET_PDEATHSIG, SIGHUP);
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }

    if(signal(SIGTERM, traffic_analyzer_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }
    if(signal(SIGHUP, traffic_analyzer_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }
    if(signal(SIGALRM, traffic_analyzer_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }

    ret = close(pipe_broadcaster_to_traffanalz_fd[PIPE_WRITE]);
    if(ret < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
    }

    Linked_list* throughput_recorder = linked_list_new();
    char recBuffer[7];
    memset(recBuffer, 0, 7);
    int is_odd = 0;
    alarm(1);

    while(1){
        ret = read(pipe_broadcaster_to_traffanalz_fd[PIPE_READ], recBuffer, 6);
        if(ret>0){
            traffic_analyzer_updatelist(throughput_recorder, atoi(recBuffer), is_odd);
        }

        if(work){
                traffic_analyzer_print_troughput(throughput_recorder);
                throughput_record_zeroize(throughput_recorder, !is_odd);
                work = 0;
                is_odd = is_odd ? 0 : 1;
                alarm(1);
        }
    }
    exit(0);
}

void traffic_analyzer_signalHandler(int signum){
    switch (signum){
        case SIGHUP:
            exit(-1);
            break;
        case SIGTERM:
            exit(0);
            break;
        case SIGALRM:
            work = 1;
            break;
            return;
        default:
            exit(-1);
            break;
    }
    return;
}
