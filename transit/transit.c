/**
 * Author: Manuel Di Pietro - dipietro.1985969@studenti.uniroma1.it
 * Date: 19-04-2024
 * Object: Transit
 * For compile this code: gcc transit.c ADT/scl.c ADT/scl_ts.c ADT/iterator.c ADT/adt_utility.c -o transit
*/
/*
 * This program simulates the operation of a railway station.
 * It takes as input the values N (number of platforms), tmin and tmax,
 * which represent the minimum and maximum arrival times between two trains
 * (the values in the range are equally probable) and the residence time
 * of each train (equal for all trains ). It uses a shared statistics structure
 * that contains the number of trains waiting, passing, and passing. Furthermore,
 * it uses a blocking queue (ADT) to guarantee that trains are served in order
 * of arrival (FIFO approach) and to guarantee mutual exclusion on operations
 * on the queue itself.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#include "ADT/scl_ts.h"

typedef struct Train{
    int id;
}Train;
void* train_new(int id);
void train_delete(Train* train);

typedef struct Statistics{
    sem_t sem;
    int crossing;
    int crossed;
    int waiting;
}Statistics;
Statistics* statistics_new();
void statistics_set_waiting_train(Statistics* statistics);
void statistics_set_crossing_train(Statistics* statistics);
void statistics_set_crossed_train(Statistics* statistics);
void statistics_delete(Statistics* statistics);

int msleep(long msec);
long long get_timestamp_ms();
void print_error(const char* file_name, int line, const char* function_name, const char* error_description);

void* transit(void*);

//As an improvement avoid use of global variables
SclTS* arrival_queue;
Statistics* stat;
int T, tmax, tmin;

int main(int argc, char* argv){
    srand(time(NULL));
    int ret;
    int platforms;
    arrival_queue = sclTS_new();
    stat = statistics_new();

    printf("Insert the value:\n");
    printf("Number of tracks (N): ");
    scanf("%d", &platforms);
    printf("Dwell time of each train (T): ");
    scanf("%d", &T);    
    printf("Minimum time between two trains (Tmin): ");
    scanf("%d", &tmin);    
    printf("Maximum time between two trains (Tmax): ");
    scanf("%d", &tmax);

    pthread_t* threads = malloc(sizeof(pthread_t)*platforms);
    //Platform generation
    for(int i=0; i<platforms; i++){
        ret = pthread_create(threads+i, NULL, transit, NULL);
        if(ret != 0){
            print_error(__FILE__, __LINE__, __func__, "Error during threads creation");
            exit(-1);
        }
    }

    //Train generation:
    int lastId = 0;
    while(1){
        int rand_value = rand() % (tmax - tmin + 1) + tmax;
        msleep(rand_value);
        statistics_set_waiting_train(stat);
        sclTS_append(arrival_queue, (void*) train_new(lastId));
        lastId += 1;
    }
    return 0;
}

void* transit(void* args){
    Train* train;
    while(1){
        train = (Train*) sclTS_remove(arrival_queue);
        if(train != NULL){
            statistics_set_crossing_train(stat);
            msleep(T);
            statistics_set_crossed_train(stat);
        }
    }
    return NULL;
}

void* train_new(int id){
    Train* train = malloc(sizeof(Train));
    train->id = id;
    return train;
}

void train_delete(Train* train){
    free(train);
    return;
}

Statistics* statistics_new(){
    int ret;
    Statistics* statistics = malloc(sizeof(Statistics));
    ret = sem_init(&statistics->sem, 0, 1);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore init");
        exit(-1);
    }
    statistics->waiting = 0;
    statistics->crossed = 0;
    statistics->crossing = 0;
    return statistics;
}

void statistics_set_waiting_train(Statistics* statistics){
    int ret;
    ret = sem_wait(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore wait");
        exit(-1);
    }
    statistics->waiting += 1;
    printf("T:%lld|WING|Waiting: %6d, crossing: %6d, crossed: %6d\n", get_timestamp_ms(), statistics->waiting, statistics->crossing, statistics->crossed);
    ret = sem_post(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore post");
        exit(-1);
    }
    return;
}

void statistics_set_crossing_train(Statistics* statistics){
    int ret;
    ret = sem_wait(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore wait");
        exit(-1);
    }
    statistics->waiting -= 1;
    statistics->crossing += 1;
    printf("T:%lld|CING|Waiting: %6d, crossing: %6d, crossed: %6d\n", get_timestamp_ms(), statistics->waiting, statistics->crossing, statistics->crossed);
    ret = sem_post(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore post");
        exit(-1);
    }
    return;
}

void statistics_set_crossed_train(Statistics* statistics){
    int ret;
    ret = sem_wait(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore wait");
        exit(-1);
    }
    statistics->crossing -= 1;
    statistics->crossed += 1;
    printf("T:%lld|CRED|Waiting: %6d, crossing: %6d, crossed: %6d\n", get_timestamp_ms(), statistics->waiting, statistics->crossing, statistics->crossed);
    ret = sem_post(&statistics->sem);
    if(ret != 0){
        print_error(__FILE__, __LINE__, __func__, "Error during statistics' semaphore post");
        exit(-1);
    }
    return;
}

void statistics_delete(Statistics* statistics){
    sem_destroy(&statistics->sem);
    free(statistics);
    return;
}

int msleep(long msec){
    struct timespec ts;
    int res;
    if (msec < 0){
        errno = EINVAL;
        return -1;
    }
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    do{
        res = nanosleep(&ts, &ts);
    }while (res && errno == EINTR);
    return res;
}

long long get_timestamp_ms(){
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000 + (long long)ts.tv_nsec / 1000000;
}

void print_error(const char* file_name, int line, const char* function_name, const char* error_description){
    printf("In function %s (file: %s - line: %d):\n", function_name, file_name, line);
    printf("\texecution error: %s\n", error_description);
    perror("\terrno");
}