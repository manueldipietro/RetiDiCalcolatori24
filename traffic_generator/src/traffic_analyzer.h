#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <string.h>


#include <sys/time.h>

#include "utility.h"
#include "scl.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct Throughput_record{
    int node_id;
    int second_odd;
    int second_even;
}Throughput_record;

void traffic_analyzer(int pipe_broadcaster_to_traffanalz_fd[2]);
void traffic_analyzer_signalHandler(int signum);

Throughput_record* throughput_record_new(int node_id, int is_odd);
Throughput_record* throughput_record_update(Throughput_record* record, int is_odd);
void throughput_record_destroyer(Throughput_record* record);
void throughput_record_print_T(void* T);
int throughput_record_compar(const void* e1, const void* e2);


void throughput_record_zeroize_aux(Linked_list_node* node, int is_odd);
void throughput_record_zeroize(Linked_list* list, int is_odd);

void traffic_analyzer_updatelist(Linked_list* list, int node_id, int is_odd);
void traffic_analyzer_print_troughput_aux(Linked_list_node* node);
void traffic_analyzer_print_troughput(Linked_list* list);