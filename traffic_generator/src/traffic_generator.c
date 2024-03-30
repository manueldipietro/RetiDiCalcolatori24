#include "traffic_generator.h"

void traffic_generator(int node_id, int pipe_trafficgen_to_broadcaster_fd[2]){   
    int ret;
    ret = prctl(PR_SET_PDEATHSIG, SIGHUP);
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }

    if(signal(SIGTERM, traffic_generator_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }
    if(signal(SIGHUP, traffic_generator_signalHandler) == SIG_ERR){
        print_error(__FILE__, __LINE__, __func__, "Unable to set signal handling!");
        exit(-1);
    }

    ret = close(pipe_trafficgen_to_broadcaster_fd[PIPE_READ]);
    if(ret < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
    }

    Traffic_generator data;
    data.node_id = node_id;
    data.pipe_trafficgen_to_broadcaster_fd = pipe_trafficgen_to_broadcaster_fd[PIPE_WRITE];
    data.sequence_number = rand();
    
    Payload random_payload;
    Message generated_message;
    char message_sendable[MESSAGE_LEN+1];
    
    while(1){
        generate_random_payload(&random_payload);
        prepare_message(&generated_message, data.node_id, data.sequence_number, &random_payload);
        message_to_string(&generated_message, message_sendable, MESSAGE_LEN);
        ret = write(data.pipe_trafficgen_to_broadcaster_fd, message_sendable, MESSAGE_LEN);
        if(ret<0){
            print_error(__FILE__, __LINE__, __func__, "Unable to write on PIPE between processes!");
        }
        data.sequence_number += 1;
        
        msleep(GENERATION_INTERVALL_MS);
    }
    printf("Arrivo qui?");

    exit(0);
}

void traffic_generator_signalHandler(int signum){
    switch (signum){
        case SIGHUP:
            exit(-1);
        case SIGTERM:
            exit(0);
        default:
            exit(-1);
    }
    return;
}
