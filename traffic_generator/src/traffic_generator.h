#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "utility.h"
#include "message.h"

#define PIPE_READ   0
#define PIPE_WRITE  1

#define GENERATION_INTERVALL_MS 10

typedef struct Traffic_generator{
    int node_id;
    int sequence_number;
    int pipe_trafficgen_to_broadcaster_fd;
}Traffic_generator;

void traffic_generator(int node_id, int pipe_trafficgen_to_broadcaster_fd[2]);
void traffic_generator_signalHandler(int signum);