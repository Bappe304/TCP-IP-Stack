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
static char send_buffer[MAX_PACKET_BUFFER_SIZE];

static unsigned int
get_next_udp_port_number(){

    return udp_port_number++;
}

static void
_pkt_receive(node_t * receiving_node, char* pkt_with_aux_data, 
                unsigned int pkt_size)
{
    char *recv_intf_name = pkt_with_aux_data;
    interface_t *recv_intf = get_node_if_by_name(receiving_node, recv_intf_name);

    if(!recv_intf)
    {
        printf("Error : Pkt recvd on unknown interface %s on node %s\n",
                    recv_intf->if_name, receiving_node->node_name);
        return;
    }

    pkt_receive(receiving_node, recv_intf, pkt_with_aux_data+IF_NAME_SIZE,
                pkt_size-IF_NAME_SIZE);

}



static int
_send_pkt_out(int sock_fd, char *pkt_data, unsigned int pkt_size,
                unsigned int dst_udp_port_no){

        int rc;
        struct sockaddr_in dest_addr;

        struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1");
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = dst_udp_port_no;
        dest_addr.sin_addr = *((struct in_addr *)host->h_addr);

        rc = sendto(sock_fd, pkt_data, pkt_size, 0,
                    (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));

        return rc;
    
}

static void *
_network_start_pkt_receiver_thread(void *arg){

    node_t *node;
    glthread_t  *curr;

    fd_set backup_sock_fd_set; /* This is the set of all the socket descriptors that we have or that the nodes in our topology have*/
    fd_set active_sock_fd_set; /* This is the set of only the socket descriptors that get ready and the set is updated continuously*/

    int sock_max_fd = 0; /* Maximum numbers of socket descriptors in the backup set*/
    int bytes_recvd = 0;

    graph_t *topo = (graph_t *)arg;
    int addr_len = sizeof(struct sockaddr); /*Size of the structure of the socket addresses*/

    FD_ZERO(&backup_sock_fd_set);
    FD_ZERO(&active_sock_fd_set);

    struct sockaddr_in sender_addr;

/* Here we iterate over the every node in our topology and add each of its socket descriptors to the backup set*/
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr)
    {
        node = graph_glue_to_node(curr);

        if(!node->udp_sock_fd) /* Check if the socket descriptor is valid*/
            continue;

        if(node->udp_sock_fd > sock_max_fd) /* Updating the number of the max socket descriptor*/
        {
            sock_max_fd = node->udp_sock_fd;
        }    
        
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
            if(FD_ISSET((int)node->udp_sock_fd, &active_sock_fd_set))
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
network_start_pkt_receiver_thread(graph_t *topo){

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr,
                    _network_start_pkt_receiver_thread,(void *)topo);
}



/*Function for sending out packet and creating the auxilary data packet*/
int
send_pkt_out(char *pkt, unsigned int pkt_size,
             interface_t *interface)
{

    int rc = 0;
    node_t *sending_node = interface->att_node;
    node_t *nbr_node = get_nbr_node(interface); /*This is the node which is going to recieve the packet*/

    if(!nbr_node)
        return -1;

    unsigned int dst_udp_port_no = nbr_node->udp_port_number; /*Target node port number*/

    /*We set a new socket to facilitate the sending of the packets*/
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
    {
        printf("Error : Sending socket creation failed, errno = %d", errno);
        return -1;
    }

    /*Here we are finding out the interface of the recieving node, so that we can add this auxilary info in the data packet*/
    interface_t *target_intf = &interface->link->intf1 == interface ? \
                                &interface->link->intf2 : &interface->link->intf1;
    
    memset(send_buffer,0,MAX_PACKET_BUFFER_SIZE);

    /* Basically creating the auxilary data packet*/
    char *pkt_with_aux_data = send_buffer;
    strncpy(pkt_with_aux_data, target_intf->if_name, IF_NAME_SIZE);
    pkt_with_aux_data[IF_NAME_SIZE] = '\0';
    memcpy(pkt_with_aux_data+IF_NAME_SIZE, pkt, pkt_size);


    rc = _send_pkt_out(sock, pkt_with_aux_data, pkt_size + IF_NAME_SIZE,
                       dst_udp_port_no);
    
    close(sock);
    return rc;
}

int
pkt_receive(node_t* node, interface_t *interface,
            char *pkt, unsigned int pkt_size)
{
    /**/
    printf("msg recvd = %s, on node = %s, IIF = %s\n", pkt, node->node_name, interface->if_name);
    return 0;
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