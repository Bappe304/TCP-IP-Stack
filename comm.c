#include "comm.h"
#include "graph.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

static unsigned int udp_port_number = 40000;

static unsigned int
get_next_udp_port_number(){

    return udp_port_number++;
}

void
init_udp_socket(node_t *node)
{
    node->udp_port_number = get_next_udp_port_number();
    int udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in node_addr;
    node_addr.sin_family = AF_INET;
    node_addr.sin_port = node->udp_port_number;
    node_addr.sin_addr.s_addr = INADDR_ANY; /*This is baically the IP address of the node*/
    if(bind(udp_sock_fd, (struct sockaddr *)&node_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Error : socket bind failed for Node %s\n", node->node_name);
        return; 
    }

    node->udp_sock_fd = udp_sock_fd;
}



void
network_start_pkt_receiver_thread(graph_t *topo){

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr,
                    network_start_pkt_receiver_thread,
                    (void *)topo);

}