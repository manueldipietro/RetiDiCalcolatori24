#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <time.h>

#include "utility.h"
#include "traffic_generator.h"
#include "broadcaster.h"
#include "traffic_analyzer.h"

#define PIPE_READ   0
#define PIPE_WRITE  1

#define PORT 9999

void broadcaster_signalHandler(int signum);

int main(int argc, char* argv[]){

    int node_id, ret;
    
    pid_t traffic_generator_pid = 0;
    pid_t traffic_analyzer_pid = 0;

    int pipe_trafficgen_to_broadcaster_fd[2];
    int pipe_broadcaster_to_traffanalz_fd[2];

    if(argc!=2){
        print_error(__FILE__, __LINE__, __func__, "Argument number error!");
        exit(-1);
    }
    node_id = atoi(argv[1]);
    node_id = node_id%1000000;

    setvbuf(stdout, NULL, _IONBF, 0);
    srand(time(NULL));

    //Installo i segnali
    if(signal(SIGALRM, broadcaster_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }
    if(signal(SIGCHLD, broadcaster_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }

    //PIPE1:
    if( pipe(pipe_trafficgen_to_broadcaster_fd) == -1 ){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
        exit(-1);
    }
    if(fcntl( pipe_trafficgen_to_broadcaster_fd[PIPE_READ], F_SETFL, fcntl(pipe_trafficgen_to_broadcaster_fd[PIPE_READ], F_GETFL) | O_NONBLOCK) < 0 ){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes as non-blocking!");
        exit(-1);
    }
    //PIPE2:
    if( pipe(pipe_broadcaster_to_traffanalz_fd) == -1 ){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
        exit(-1);
    }
    if(fcntl(pipe_broadcaster_to_traffanalz_fd[PIPE_READ], F_SETFL, fcntl(pipe_broadcaster_to_traffanalz_fd[PIPE_READ], F_GETFL) | O_NONBLOCK) < 0 ){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes as non-blocking!");
        exit(-1);
    }

    //Genero i processi figli
    traffic_generator_pid = fork();
    if(traffic_generator_pid > 0){
        traffic_generator(node_id, pipe_trafficgen_to_broadcaster_fd);
        exit(0);
    }
    if(traffic_generator_pid < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to create subprocess!");
        exit(-1);
    }

    traffic_analyzer_pid = fork();
    if(traffic_analyzer_pid > 0){
        traffic_analyzer(pipe_broadcaster_to_traffanalz_fd);
        exit(0);
    }
    if(traffic_analyzer_pid < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to create subprocess!");
        exit(-1);
    }

    //Setto l'end della simulazione
    alarm(20);

    broadcaster_process(node_id, PORT, pipe_trafficgen_to_broadcaster_fd, pipe_broadcaster_to_traffanalz_fd);

    return 0;
}

void broadcaster_signalHandler(int signum){
    switch (signum){
        case SIGCHLD:
            exit(0);
        case SIGALRM:
            exit(0);
            break;
        default:
            exit(-1);
    }
    return;
}