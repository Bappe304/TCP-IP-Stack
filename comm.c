#include "comm.h"
#include "graph.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

static unsigned int udp_port_number = 40000;
static char recv_buffer[MAX_PACKET_BUFFER_SIZE];

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


static void *
_network_start_pkt_receiver_thread(void *arg){

    node_t *node;
    glthread_t  *curr;

    fd_set backup_sock_fd_set, /* This is the set of all the socket descriptors that we have or that the nodes in our topology have*/
           active_sock_fd_set; /* This is the set of only the socket descriptors that get ready and the set is updated continuously*/

    int sock_max_fd = 0; /* Maximum numbers of socket descriptors in the backup set*/
    int bytes_recvd = 0;

    graph_t *topo = (void *)arg;

    int addr_len = sizeof(struct sockaddr); /*Size of the structure of the socket addresses*/

    FD_ZERO(&backup_sock_fd_set);
    FD_ZERO(&active_sock_fd_set);

    struct sockaddr_in sender_addr;

/* Here we iterate over the every node in our topology and add each of its socket descriptors to the backup set*/
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){
        node = graph_glue_to_node(curr);

        if(!node->udp_sock_fd) /* Check if the socket descriptor is valid*/
            continue;

        if(node->udp_sock_fd > sock_max_fd) /* Updating the number of the max socket descriptor*/
            sock_max_fd = node->udp_sock_fd;
        
        FD_SET(node->udp_sock_fd, &backup_sock_fd_set); /*Updating all of these descriptors in the backupset on every iteration*/
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);

    while(1)
    {
        /*Copying the entire list of socket descriptors from backup-set to the active-set.
        *We copy the entire list again at every iteration just to keep conitnuosly checking
        *which socket is ready for the I/O operation*/
        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));

        /*This is where we mainly check the sockets for readiness, basically here the kernel takes over
        *and keeps waiting for activity on these sockets either upto a timelimit or indefenitely, depending
        *on the application*/
        select(sock_max_fd+1, &active_sock_fd_set, NULL, NULL, NULL);
        /*Select function is a blocking function i.e. until any of the sockets is ready no further code would be run*/


        /* We iterate on all the nodes in topology again to check if the socket of each of these node is in the
        *active-set, incase it is active than we just copy the data and then pass it on to be processed*/
        ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr)
        {
            node = graph_glue_to_node(curr);

            /*Checking if the socket is in the active-set*/
            if(FD_ISSET(node->udp_sock_fd, &active_sock_fd_set))
            {
                
                memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE); /*Clearing the buffer of any data in the previous iteration*/
                bytes_recvd = recvfrom(node->udp_sock_fd, (char *)recv_buffer,
                              MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &addr_len); /*Basically just updates the number of bytes being copied*/

                _pkt_receive(node, recv_buffer, bytes_recvd); /*Processes the data according to the application logic*/
            }
        }
    }
}




void
network_start_pkt_receiver_thread(graph_t *topo)
{

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr,
                    network_start_pkt_receiver_thread,
                    (void *)topo);

}