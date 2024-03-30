#include "broadcaster.h"

void broadcaster_process(int node_id, int port, int pipe_trafficgen_to_broadcaster_fd[2], int pipe_broadcaster_to_traffanalz_fd[2]){
    int ret;
    Broadcaster data;
    char recBuffer[MESSAGE_LEN+1];
    int received_id, received_seq;


    //prepare first pipe
    ret = close(pipe_trafficgen_to_broadcaster_fd[PIPE_WRITE]);
    if(ret < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
    }

    ret = close(pipe_broadcaster_to_traffanalz_fd[PIPE_READ]);
    if(ret < 0){
        print_error(__FILE__, __LINE__, __func__, "Unable to set PIPE between processes!");
    }


    data.node_id = node_id;
    data.port = port;
    Linked_list* sended_record = linked_list_new();
    memset(recBuffer, 0, MESSAGE_LEN+1);

    broadcaster_openudpsocket(&data);
    
    char id_string[7];
    snprintf(id_string, 7, "%06d", data.node_id);

    while(1){
        ret = read(pipe_trafficgen_to_broadcaster_fd[PIPE_READ], recBuffer, MESSAGE_LEN);
        if(ret>0){
            broadcaster_sendudpmessage(&data, recBuffer, strlen(recBuffer));
            ret = write(pipe_broadcaster_to_traffanalz_fd[PIPE_WRITE], id_string, 6);
            memset(recBuffer, 0, MESSAGE_LEN+1);
        }
        ret = 0;

        ret = broadcaster_receiveudpmessage(&data, recBuffer, MESSAGE_LEN);
        if(ret>0){
            int x = regex_message_validation("Src=([0-9]{6});Seq=([0-9]{6});Pay=([0-9]{6});\n", recBuffer);
            if(x){
                received_id = strtol(&recBuffer[4], NULL, 10);
                received_seq = strtol(&recBuffer[15], NULL, 10);
                if(data.node_id != received_id){
                    if(broadcaster_is_sendable(sended_record, received_id, received_seq)){
                        broadcaster_sendudpmessage(&data, recBuffer, strlen(recBuffer));
                        ret = write(pipe_broadcaster_to_traffanalz_fd[PIPE_WRITE], &recBuffer[4], 6);
                    }
                }
            }
        }
        memset(recBuffer, 0, MESSAGE_LEN+1);
        ret = 0;

    }
    
    exit(0);
}


int broadcaster_openudpsocket(Broadcaster* data){
    int ret;
    data->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(data->socket_fd < 0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to open broadcast UDP socket!");
        return -1;
    }
    int broadcast = 1;
    ret = setsockopt(data->socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to set socket option!");
        return -1;
    }
    memset((void*)&(data->ingress_address), 0, sizeof(struct sockaddr_in));
    (data->ingress_address).sin_family = AF_INET;
    (data->ingress_address).sin_port = htons(data->port);
    (data->ingress_address).sin_addr.s_addr = INADDR_ANY;
    ret = bind(data->socket_fd, (struct sockaddr*)&(data->ingress_address), sizeof(struct sockaddr));
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to bind receiving socket!");
        return -1;
    }
    memset((void*)&(data->egress_address), 0, sizeof(struct sockaddr_in));
    (data->egress_address).sin_family = AF_INET;
    (data->egress_address).sin_port = htons(data->port);
    (data->egress_address).sin_addr.s_addr = INADDR_BROADCAST;
    return 0;
}

int broadcaster_sendudpmessage(Broadcaster* data, char* message, int message_len){
    int ret;
    struct ifaddrs *addrs, *tmp;
    ret = getifaddrs(&addrs);
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to get list of physical interfaces!");
        return -1;
    }
    tmp = addrs;
    while(tmp){
        if(tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET){
            ret = setsockopt(data->socket_fd, SOL_SOCKET, SO_BINDTODEVICE, tmp->ifa_name, sizeof(tmp->ifa_name));
            if(ret<0){
                print_error(__FILE__, __LINE__, __func__, "Socket: Unable to bind with physical interfaces!");
                return -1;
            }
            
            //This is necessary because there is a bug in recvfrom and the address will be overwritten
            (data->egress_address).sin_family = AF_INET;
            (data->egress_address).sin_port = htons(data->port);
            (data->egress_address).sin_addr.s_addr = INADDR_BROADCAST;
            //ENDBUGCORRECTION

            ret = sendto(data->socket_fd, message, message_len, 0, (struct sockaddr*)&((data->egress_address)), sizeof(data->egress_address));
            if(ret<0){
                print_error(__FILE__, __LINE__, __func__, "Socket: Unable to send data on socket!");
                return -1;
            }
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    ret = setsockopt(data->socket_fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0);
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to restore socket for listen!");
        return -1;
    }
    return 0;
}

int broadcaster_receiveudpmessage(Broadcaster* data, char* buffer, int buffer_len){
    //TO_DO: Va aggiunto controllo errore su select
    int recvlen = 0;
    struct timeval t = {0, 0};
    fd_set s_fd;
    FD_ZERO(&s_fd);
    FD_SET(data->socket_fd, &s_fd);
    if( select(data->socket_fd+1, &s_fd, NULL, NULL, &t) > 0){
        int addr_len = sizeof(struct sockaddr*);
        recvlen = recvfrom(data->socket_fd, buffer, buffer_len, 0, (struct sockaddr*)&(data->egress_address), &(addr_len));
        if(recvlen < 0){
            print_error(__FILE__, __LINE__, __func__, "Socket: Unable to receive!");
            return -1;
        }
    }

    return recvlen;
}

int broadcaster_closeudpsocket(Broadcaster* data){
    int ret;
    ret = shutdown(data->socket_fd, SHUT_RDWR);
    if(ret<0){
        print_error(__FILE__, __LINE__, __func__, "Socket: Unable to close socket!");
        return -1;
    }
    return 0;
}

int broadcaster_is_sendable(Linked_list* list, int node_id, int sequence){
    Sended_record ra = {node_id, sequence};

    Sended_record* finded = (Sended_record* ) linked_list_search_by_value(list, &ra, sended_record_compar);

    if(finded==NULL){
        Sended_record* new_record = sended_record_new(node_id, sequence);
        linked_list_add(list, (void*) new_record);
        return 1;
    }

    if( finded->last_sequence < sequence ){
        sended_record_update(finded, sequence);
        return 1;
    }

    return 0;
}
