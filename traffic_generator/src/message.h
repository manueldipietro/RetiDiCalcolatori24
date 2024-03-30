#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

#define MESSAGE_LEN 34

typedef struct Message {
    int source;
    int sequence;
    int payload_lenght;
    char * payload;
} Message;

typedef struct Payload {
    int value;
} Payload;

void generate_random_payload(Payload* payload);
void prepare_message(Message* message, int src_node, int seq_number, Payload * payload);
void message_to_string(Message* message, char* string, int max_len);
int regex_message_validation(char* expression, char* string);
