#include "command.h"

Command* command_new(int command_id, int sequence_number){
    Command* command = malloc(sizeof(Command));
    
    command->command_id = command_id;
    command->sequence_number = sequence_number;

    switch(command->command_id){
        case UNRECOGNIZED:
            command->command_params = NULL;
        case CLIENT_SETINFO:
            command->command_params = malloc(sizeof(Command_client_setinfo_params));
        break;
        case SET:
            command->command_params = malloc(sizeof(Command_set_params));
        break;
        case GET:
            command->command_params = malloc(sizeof(Command_get_params));
        break;
        //default:
        //    command->command_params = NULL;
    }
    return command;
}

void command_delete(Command* command){
    switch(command->command_id){
        case UNRECOGNIZED:
        break;
        case CLIENT_SETINFO:
            Command_client_setinfo_params* params1 = (Command_client_setinfo_params*) command->command_params;
            free(params1);
        break;
        case GET:
            Command_get_params* params2 = (Command_get_params*) command->command_params;
            free(params2);
        break;
        case SET:
            Command_set_params* params3 = (Command_set_params*) command->command_params;
            free(params3);
        break;

    }
    free(command);
    return;
}

Command* command_parser(Resp2_array* array, int sequence_number){
    Command* command = NULL;
    
    command = command_is_client_setinfo(array, sequence_number);
    if(command != NULL){
        return command;
    }
    command = command_is_get(array, sequence_number);
    if(command != NULL){
        return command;
    } 

    command = command_is_set(array, sequence_number);
    if(command != NULL){
        return command;
    } 

    return command_new(UNRECOGNIZED, sequence_number);
}

void* command_worker_args_new(Client* client, Command* command){
    Command_worker_args* args = malloc(sizeof(Command_worker_args*));
    args->client = client;
    args->command = command;
    return args;
}

void* command_worker_unrecognized(void* args){
    Command_worker_args* worker_args = (Command_worker_args*) args;
    Resp2_error* response_resp2 = resp2_error_new(RESP2_ERROR_GENERIC, "Unrecognized command!", 21);
    char* response_message; int response_len;
    response_len = resp2_error_toStr(response_resp2, &response_message);
    client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);
    command_delete(worker_args->command);
    free(worker_args);
    pthread_exit(NULL);
}

Command* command_is_client_setinfo(Resp2_array* array, int local_progressive_number){
    int f = 0;
    if(array->element_number == 4){
        Resp2_bulkstring* tmp;
        
        if(!f && resp2_array_get_type(array, 0) == RESP2_BULKSTRING){
            tmp = resp2_array_get_data(array, 0);
            if(tmp->lenght == 6){
                f = (strncmp(tmp->data, "CLIENT", 6) == 0 ? f : 1);
            }else f = 1;
        }else f = 1;

        if(!f  && resp2_array_get_type(array, 1) == RESP2_BULKSTRING){
            tmp = resp2_array_get_data(array, 1);
            if(tmp->lenght == 7){
                f = (strncmp(tmp->data, "SETINFO", 6) == 0 ? f : 1);
            }else f = 1;
        }else f = 1;
        if(!f  && resp2_array_get_type(array, 2) != RESP2_BULKSTRING) f = 1;
        if(!f  && resp2_array_get_type(array, 3) != RESP2_BULKSTRING) f = 1;
        if(!f){
            Command* command = command_new(CLIENT_SETINFO, local_progressive_number);
            ( (Command_client_setinfo_params*) command->command_params) -> to_set = resp2_bulkstring_new(((Resp2_bulkstring*)resp2_array_get_data(array, 2))->data, ((Resp2_bulkstring*)resp2_array_get_data(array, 2))->lenght);
            ( (Command_client_setinfo_params*) command->command_params) -> value = resp2_bulkstring_new(((Resp2_bulkstring*)resp2_array_get_data(array, 3))->data, ((Resp2_bulkstring*)resp2_array_get_data(array, 3))->lenght);
            return command;
        }
    }
    return NULL;
}

void* command_worker_clientinfo_set(void* args){
    Command_worker_args* worker_args = (Command_worker_args*) args;
    Command_client_setinfo_params* params = (Command_client_setinfo_params*) (worker_args->command->command_params);
    char* response_message; int response_len;

    if(params->to_set->lenght == 7){
        if( strncmp(params->to_set->data, "LIB-VER", 7))
            client_set_libver(worker_args->client, params->value);
        
        Resp2_string* response = resp2_string_new("OK", 2);
        response_len = resp2_string_toStr(response, &response_message);
        resp2_string_delete(response);
        
    }else if(params->to_set->lenght == 8){
        if( strncmp(params->to_set->data, "LIB-NAME", 8))
            client_set_libname(worker_args->client, params->value);

        Resp2_string* response = resp2_string_new("OK", 2);
        response_len = resp2_string_toStr(response, &response_message);
        resp2_string_delete(response);
        
    }else{
        Resp2_error* response = resp2_error_new(RESP2_ERROR_GENERIC, "SET_CLIENTINFO, unvalid params!", 31);
        response_len = resp2_error_toStr(response, &response_message);
        resp2_error_delete(response);
    }

    client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);

    command_delete(worker_args->command);
    free(worker_args);
    pthread_exit(NULL);
}

Command* command_is_get(Resp2_array* array, int local_progressive_number){
    int f = 0;
    if(array->element_number == 2){
        Resp2_bulkstring* tmp;
        
        if(!f && resp2_array_get_type(array, 0) == RESP2_BULKSTRING){
            tmp = resp2_array_get_data(array, 0);
            if(tmp->lenght == 3){
                f = (strncmp(tmp->data, "GET", 3) == 0 ? f : 1);
            }else f = 1;
        }else f = 1;
        
        if(!f  && resp2_array_get_type(array, 1) != RESP2_BULKSTRING) f = 1;

        if(!f){
            Command* command = command_new(GET, local_progressive_number);
            ( (Command_get_params*) command->command_params)-> key = resp2_bulkstring_new( ((Resp2_bulkstring*) resp2_array_get_data(array, 1))->data, ((Resp2_bulkstring*) resp2_array_get_data(array, 1))->lenght);
            return command;
        }
    }
    return NULL;
}

void* command_worker_get(void* args){
    Command_worker_args* worker_args = (Command_worker_args*) args;
    Command_get_params* params = (Command_get_params*) (worker_args->command->command_params);
    char* response_message; int response_len;

    Resp2_bulkstring* response = tdbms_get(worker_args->client->tdbms, params->key);
    response_len = resp2_bulkstring_toStr(response, &response_message);   
    resp2_bulkstring_delete(response);
    
    client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);

    command_delete(worker_args->command);
    free(worker_args);
    pthread_exit(NULL);
}

Command* command_is_set(Resp2_array* array, int local_progressive_number){
    int f = 0;

    Command_set_write_mode write_mode = SWM_DEFAULT;
    int get;
    Command_set_ttl_mode ttl_mode = STM_DEFAULT;
    Resp2_bulkstring* ttl_value = NULL;

    if(array->element_number >= 3 && array->element_number <= 7){
        Resp2_bulkstring* tmp;
        
        //COMMAND
        if(!f && resp2_array_get_type(array, 0) == RESP2_BULKSTRING){
            tmp = resp2_array_get_data(array, 0);
            if(tmp->lenght == 3){
                f = (strncmp(tmp->data, "SET", 3) == 0 ? f : 1);
            }else f = 1;
        }else f = 1;

        //VALUE
        if(!f && resp2_array_get_type(array, 1) == RESP2_BULKSTRING){
        }else f = 1;

        //KEY
        if(!f && resp2_array_get_type(array, 2) == RESP2_BULKSTRING){
        }else f = 1;

        if(!f){
            for(int i=3; i<array->element_number; i++){
                if(!f && resp2_array_get_type(array, i) == RESP2_BULKSTRING){
                    tmp = (Resp2_bulkstring*) resp2_array_get_data(array, i);
                    //L==2, NX, XX, EX, PX
                    if(tmp->lenght == 2){
                        //CASO NX, XX
                        if(strncmp(tmp->data, "NX", 2) == 0){
                            if(write_mode != SWM_DEFAULT) f = 1;
                            else write_mode = SWM_NX;
                        }
                        else if(strncmp(tmp->data, "XX", 2) == 0){
                            if(write_mode != SWM_DEFAULT) f = 1;
                            else write_mode = SWM_XX;
                        }
                        //CASO EX, PX
                        else if(strncmp(tmp->data, "EX", 2) == 0){
                            if(ttl_mode != STM_DEFAULT) f = 1;
                            else ttl_mode = STM_EX;
                        }
                        else if(strncmp(tmp->data, "PX", 2) == 0){
                            if(ttl_mode != STM_DEFAULT) f = 1;
                            else ttl_mode = STM_PX;
                        }
                        //CASO CHIAVE
                        else if(ttl_mode != STM_DEFAULT && ttl_mode != STM_KEEPTTL && ttl_value == NULL){
                            for(int j=0; j<tmp->lenght; j++)
                                if(tmp->data[j] < '0' || tmp->data[j] > '9')
                                    f = 1;
                            ttl_value = (f == 1 ? NULL : tmp);
                        }
                        else f=1;
                    }
                    //L==3, GET
                    else if(tmp->lenght == 3){
                        if(strncmp(tmp->data, "GET", 3) == 0){
                            if(get > 0) f = 1;
                            else get = 1;
                        }
                        else if(ttl_mode != STM_DEFAULT && ttl_mode != STM_KEEPTTL && ttl_value == NULL){
                            for(int j=0; j<tmp->lenght; j++)
                                if(tmp->data[j] < '0' || tmp->data[j] > '9')
                                    f = 1;
                            ttl_value = (f == 1 ? NULL : tmp);
                        }
                        else f=1;
                    }
                    //L==4, EXAT, PXAT
                    else if(tmp->lenght == 4){
                        if(strncmp(tmp->data, "EXAT", 4) == 0){
                            if(ttl_mode != STM_DEFAULT) f = 1;
                            else ttl_mode = STM_EXAT;
                        }
                        else if(strncmp(tmp->data, "PXAT", 4) == 0){
                            if(ttl_mode != STM_DEFAULT) f = 1;
                            else ttl_mode = STM_PXAT;
                        }
                        else if(ttl_mode != STM_DEFAULT && ttl_mode != STM_KEEPTTL && ttl_value == NULL){
                            for(int j=0; j<tmp->lenght; j++)
                                if(tmp->data[j] < '0' || tmp->data[j] > '9')
                                    f = 1;
                            ttl_value = (f == 1 ? NULL : tmp);
                        }
                        else f=1;
                    }
                    //L==7, KEEPTTL
                    else if(tmp->lenght == 4){
                        if(strncmp(tmp->data, "KEEPTTL", 4) == 0){
                            if(ttl_mode != STM_DEFAULT) f = 1;
                            else ttl_mode = STM_KEEPTTL;
                        }
                        else if(ttl_mode != STM_DEFAULT && ttl_mode != STM_KEEPTTL && ttl_value == NULL){
                            for(int j=0; j<tmp->lenght; j++)
                                if(tmp->data[j] < '0' || tmp->data[j] > '9')
                                    f = 1;
                            ttl_value = (f == 1 ? NULL : tmp);
                        }
                        else f=1;
                    }
                    //L DIVERSA, CASO VALORE EXPYRE
                    else{
                        if(ttl_mode != STM_DEFAULT && ttl_mode != STM_KEEPTTL && ttl_value == NULL){
                            for(int j=0; j<tmp->lenght; j++)
                                if(tmp->data[j] < '0' || tmp->data[j] > '9')
                                    f = 1;
                            ttl_value = (f == 1 ? NULL : tmp);
                        }
                        else f=1;
                    }
                }else f = 1;
                if(f) return NULL;
            }
        }

        if(!f){
            Command* command = command_new(SET, local_progressive_number);
            ( (Command_set_params*) command->command_params) -> key = resp2_bulkstring_new( ((Resp2_bulkstring*) resp2_array_get_data(array, 1))->data, ((Resp2_bulkstring*) resp2_array_get_data(array, 1))->lenght);
            ( (Command_set_params*) command->command_params) -> value = resp2_bulkstring_new( ((Resp2_bulkstring*) resp2_array_get_data(array, 2))->data, ((Resp2_bulkstring*) resp2_array_get_data(array, 2))->lenght);
            ( (Command_set_params*) command->command_params) -> get = get;
            ( (Command_set_params*) command->command_params) -> write_mode = write_mode;
            ( (Command_set_params*) command->command_params) -> ttl_mode = ttl_mode;
            if(ttl_value != NULL)
                ( (Command_set_params*) command->command_params) -> ttl_value = resp2_bulkstring_new(ttl_value->data, ttl_value->lenght);
            else ( (Command_set_params*) command->command_params) -> ttl_value = NULL;
            return command;
        }
    }
    return NULL;
}

void* command_worker_set(void* args){
    int ret;
    Command_worker_args* worker_args = (Command_worker_args*) args;
    Command_set_params* params = (Command_set_params*) (worker_args->command->command_params);
    char* response_message; int response_len;
    
    //SETTING THE VALUE INTO DBMS
    Resp2_bulkstring* old_value;
    Tdbms_insert_mode mode =TDBMS_OVERWRITE;
    if(params->write_mode== SWM_NX) mode = TDBMS_ONLY_NOT_EXISTS;
    if(params->write_mode== SWM_XX) mode = TDBMS_ONLY_EXISTS;

    if(tdbms_set(worker_args->client->tdbms, params->key, params->value, mode, &old_value)!=0){  
        //SETTING THE TIMESTAMP

        //CASO NULLO, PROVO AD ELIMINARE IL RECORD
        if(params->ttl_mode != STM_KEEPTTL){
            expirer_manager_remove(worker_args->client->expirer_manager, params->key);

            if(params->ttl_value != NULL){ 
                char* endptr = &(params->ttl_value->data[params->ttl_value->lenght]);
                long ttl_value = strtol( params->ttl_value->data, &(endptr) , 10);
                struct timespec actual_time;
                struct timespec to_add;
                ret = clock_gettime(CLOCK_REALTIME, &actual_time);
                if(ret != 0){
                        print_error(__FILE__, __LINE__, __func__, "Error during gettint actual time!");
                        exit(-1);
                }
                if(params->ttl_mode == STM_EX){
                    to_add = timespec_from_ms(ttl_value*1000);
                    actual_time = timespec_add(actual_time, to_add);
                    expirer_manager_append(worker_args->client->expirer_manager, params->key, actual_time);
                }
                if(params->ttl_mode == STM_PX){
                    to_add = timespec_from_ms(ttl_value);
                    actual_time = timespec_add(actual_time, to_add);
                    expirer_manager_append(worker_args->client->expirer_manager, params->key, actual_time);
                }
                if(params->ttl_mode == STM_EXAT){
                    to_add = timespec_from_ms(ttl_value*1000);
                    expirer_manager_append(worker_args->client->expirer_manager, params->key, actual_time);
                }
                if(params->ttl_mode == STM_PXAT){
                    actual_time = timespec_from_ms(ttl_value);
                expirer_manager_append(worker_args->client->expirer_manager, params->key, actual_time);
                }
            }
        }

        if(params->get == 1){
            if(old_value == NULL){
                Resp2_bulkstring* null_respose;
                response_len = resp2_bulkstring_toStr(null_respose, &response_message);
                resp2_bulkstring_delete(null_respose);
        
                client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);

                command_delete(worker_args->command);
                free(worker_args);
                pthread_exit(NULL);
            }
            response_len = resp2_bulkstring_toStr(old_value, &response_message);
            resp2_bulkstring_delete(old_value);

            client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);

            command_delete(worker_args->command);
            free(worker_args);
            pthread_exit(NULL);
        }else{
            Resp2_string* ok_response = resp2_string_new("OK", 2);
            response_len = resp2_string_toStr(ok_response, &response_message);
            resp2_string_delete(ok_response);

            client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);
        
            command_delete(worker_args->command);
            free(worker_args);
            pthread_exit(NULL);
        }
    }else{
        Resp2_bulkstring* null_respose;
        response_len = resp2_bulkstring_toStr(null_respose, &response_message);
        resp2_bulkstring_delete(null_respose);
        
        client_send_response(worker_args->client, worker_args->command->sequence_number, response_message, response_len);

        command_delete(worker_args->command);
        free(worker_args);
        pthread_exit(NULL);
    }
    pthread_exit(NULL);
}
