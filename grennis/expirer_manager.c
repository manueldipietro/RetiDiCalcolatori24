 #include "expirer_manager.h"

Expirer_manager_record* expirer_manager_record_new(Resp2_bulkstring* key, struct timespec timestamp){
    Expirer_manager_record* record = malloc(sizeof(Expirer_manager_record));
    record->key = resp2_bulkstring_new(key->data, key->lenght);
    record->timestamp = timestamp;
    return record;
}

int expirer_manager_record_cmp(const void* e1, const void* e2){
    Expirer_manager_record* r1 = (Expirer_manager_record*) e1;
    Expirer_manager_record* r2 = (Expirer_manager_record*) e2;
    return resp2_bulkstring_cmp(r1->key, r2->key);
}

int expirer_manager_timestamp_cmp(const void* e1, const void* e2){
    Expirer_manager_record* r1 = (Expirer_manager_record*) e1;
    Expirer_manager_record* r2 = (Expirer_manager_record*) e2;
    return timespec_cmp(r1->timestamp, r2->timestamp); 
}

void expirer_manager_record_delete(void* record){
    Expirer_manager_record* record_casted = (Expirer_manager_record*) record;
    resp2_bulkstring_delete(record_casted->key);
    free(record_casted);
    return;
}

void* expirer_manager_process(void* args);

Expirer_manager* expirer_manager_new(TDBMS* tdbms){
    int ret;    
    Expirer_manager* expirer_manager = malloc(sizeof(Expirer_manager));
    
    expirer_manager->queue = scl_ordered_new(expirer_manager_timestamp_cmp);
    expirer_manager->tdbms = tdbms;

    ret = pthread_cond_init(&expirer_manager->sync_condition, NULL);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during expirer_manager condition_variable creation!");
        exit(-1);
    }
    ret = pthread_mutex_init(&expirer_manager->sync_mutex, NULL);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during expirer_manager mutex creation!");
        exit(-1);
    }

    ret = pthread_create(&(expirer_manager->thread_descriptor), 0, expirer_manager_process, expirer_manager);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during server thread creation!");
        exit(-1);
    }

    return expirer_manager;
}

void* expirer_manager_process(void* args){
    int ret;
    Expirer_manager* expirer_manager = (Expirer_manager*) args;
    Expirer_manager_record* record;
    struct timespec actual_time;
    struct timespec wait_time;

    pthread_mutex_lock(&expirer_manager->sync_mutex);
    //!TODO: andrebbe aggiunto controllo errori su questa funzione
    while(1){
        if( scl_ordered_element_number(expirer_manager->queue) == 0){
            pthread_cond_wait(&expirer_manager->sync_condition, &expirer_manager->sync_mutex);
            //!TODO: andrebbe aggiunto controllo errori su questa funzione
        }else{            
            ret = clock_gettime(CLOCK_REALTIME, &actual_time);
            if(ret != 0){
                print_error(__FILE__, __LINE__, __func__, "Error during gettint actual time!");
                exit(-1);
            }

            record = (Expirer_manager_record*) scl_ordered_get(expirer_manager->queue);

            if(timespec_cmp(actual_time, record->timestamp)>=0){
                tdbms_remove(expirer_manager->tdbms, record->key);
                scl_ordered_remove_andfreeinfo(expirer_manager->queue, expirer_manager_record_delete);
            }else{
                wait_time = timespec_sub(record->timestamp, actual_time);
                pthread_cond_timedwait(&expirer_manager->sync_condition, &expirer_manager->sync_mutex, &wait_time);
                //!TODO: andrebbe aggiunto controllo errori su questa funzione
            }
        }
    }
    pthread_exit(NULL);
}

void expirer_manager_append(Expirer_manager* expirer_manager, Resp2_bulkstring* key, struct timespec timestamp){
    pthread_mutex_lock(&expirer_manager->sync_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    scl_ordered_qr_andfreeinfo(expirer_manager->queue, expirer_manager_record_new(key, timestamp), expirer_manager_record_cmp, expirer_manager_record_delete);
    scl_ordered_append(expirer_manager->queue, expirer_manager_record_new(key, timestamp));
    pthread_mutex_unlock(&expirer_manager->sync_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    pthread_cond_broadcast(&expirer_manager->sync_condition);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    return;
}

void expirer_manager_remove(Expirer_manager* expirer_manager, Resp2_bulkstring* key){
    struct timespec timestamp;
    pthread_mutex_lock(&expirer_manager->sync_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    scl_ordered_qr_andfreeinfo(expirer_manager->queue, expirer_manager_record_new(key, timestamp), expirer_manager_record_cmp, expirer_manager_record_delete);
    pthread_mutex_unlock(&expirer_manager->sync_mutex);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    pthread_cond_broadcast(&expirer_manager->sync_condition);
        //!TODO: andrebbe aggiunto controllo errori su questa funzione
    return;
}