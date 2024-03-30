#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <signal.h>

#include "utility.h"
#include "message.h"
#include "scl.h"
#include "sended_recorder.h"

#define PIPE_WRITE  1
#define PIPE_READ   0


typedef struct Broadcaster{
    int node_id;
    int socket_fd;
    int port;
    struct sockaddr_in ingress_address;
    struct sockaddr_in egress_address;
    //Linked_list* other_node_sequence;
}Broadcaster;

void broadcaster_process(int node_id, int port, int pipe_trafficgen_to_broadcaster_fd[2], int pipe_broadcaster_to_traffanalz_fd[2]);
int broadcaster_openudpsocket(Broadcaster* data);
int broadcaster_sendudpmessage(Broadcaster* data, char* message, int message_len);
int broadcaster_receiveudpmessage(Broadcaster* data, char* buffer, int buffer_len);
int broadcaster_closeudpsocket(Broadcaster* data);
int broadcaster_is_sendable(Linked_list* list, int node_id, int sequence);