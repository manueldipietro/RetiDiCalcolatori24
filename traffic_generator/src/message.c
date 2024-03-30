#include "message.h"

void generate_random_payload(Payload* payload){
    payload->value = rand()%1000000;
    return;
}

void prepare_message(Message* message, int src_node, int seq_number, Payload * payload){
    message->source = src_node;
    message->sequence = seq_number;
    message->payload_lenght = 6;
    message->payload = malloc(sizeof(char)*(message->payload_lenght+1));
    snprintf(message->payload, message->payload_lenght+1, "%06d", payload->value);
    return;
}

void message_to_string(Message* message, char* string, int string_max_len){
    snprintf(string, string_max_len+1, "Src=%06d;Seq=%06d;Pay=%s;\n", message->source%1000000, message->sequence%1000000, message->payload);
    return;
}

int regex_message_validation(char* expression, char* string){
    int ret;
    regex_t regexp;
    ret = regcomp(&regexp, expression, REG_EXTENDED);
    if(ret != 0 ){
        //Gestione errore
    }
    int match = regexec(&regexp, string, 0, NULL, 0);
    if( match != 0)
        return 0;
    return 1;
}
