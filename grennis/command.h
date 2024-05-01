#pragma once
#include <stdlib.h>
#include <time.h>

#include "resp2/resp2.h"
#include "client.h"
#include "timespec.h"
#include "expirer_manager.h"

typedef enum Command_id{
    UNRECOGNIZED, CLIENT_SETINFO, SET, GET
}Command_id;

typedef struct Command{
    Command_id command_id;
    void* command_params;
    int sequence_number;        
    int client_id;
}Command;

typedef struct Command_worker_args{
    Client* client;
    Command* command;
}Command_worker_args;

Command* command_new(int command_id, int sequence_number);
void command_delete(Command* command);
Command* command_parser(Resp2_array* array, int sequence_number);

void* command_worker_args_new(Client* client, Command* command);
void* command_worker_unrecognized(void* args);


//CLIENT_SETINFO
typedef struct Command_client_setinfo_params{
    Resp2_bulkstring* to_set;
    Resp2_bulkstring* value;
}Command_client_setinfo_params;

Command* command_is_client_setinfo(Resp2_array* array, int local_progressive_number);
void* command_worker_clientinfo_set(void* args);

typedef struct Command_get_params{
    Resp2_bulkstring* key;
}Command_get_params;

Command* command_is_get(Resp2_array* array, int local_progressive_number);
void* command_worker_get(void* args);

typedef enum Command_set_write_mode{
    SWM_DEFAULT, SWM_NX, SWM_XX
}Command_set_write_mode;

typedef enum Command_set_ttl_mode{
    STM_DEFAULT, STM_EX, STM_PX, STM_EXAT, STM_PXAT, STM_KEEPTTL
}Command_set_ttl_mode;

typedef struct Command_set_params{
    Resp2_bulkstring* key;
    Resp2_bulkstring* value;
    int get;
    Command_set_write_mode write_mode;
    Command_set_ttl_mode ttl_mode;
    Resp2_bulkstring* ttl_value;
}Command_set_params;

Command* command_is_set(Resp2_array* array, int local_progressive_number);
void* command_worker_set(void* args);