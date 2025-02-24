/*
 * Description: This file contains the basic strucutres such as node, interface, link and graphs.
 *
 *
 *
 *
*/

#ifndef __GRAPH__
#define __GRAPH__


#include "gluethread/glthread.h"
#include "stddef.h"
#include "net.h"
#include <assert.h>

#define NODE_NAME_SIZE 16
#define IF_NAME_SIZE 16
#define MAX_INTF_PER_NODE 10

/* FORWARD DECLARATION */
typedef struct node_ node_t;
typedef struct link_ link_t;



typedef struct interface_{

    char if_name[IF_NAME_SIZE];
    struct node_ *att_node;
    struct link_ *link;
    intf_nw_props_t intf_nw_props;
}interface_t;


struct link_{
    interface_t intf1;
    interface_t intf2;
    unsigned int cost;
};

struct node_{
    char node_name[NODE_NAME_SIZE];
    interface_t *intf[MAX_INTF_PER_NODE];
    node_nw_prop_t node_nw_prop;
    unsigned int udp_port_number;
    int udp_sock_fd;
    glthread_t graph_glue;
};
GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);

typedef struct graph_{
    
    char topology_name[32];
    glthread_t node_list;

}graph_t;


node_t * create_graph_node(graph_t *graph, char *node_name);

graph_t * create_new_graph(char *topology_name);

void insert_link_between_two_nodes(node_t *node1, node_t *node2, char *from_if_name, char *to_if_name, unsigned int cost);




/* API to return the node of the interface passed as in the argument */
static inline node_t * get_node(interface_t *interface)
{
    if(!interface)
    {
	    return NULL;
    }
    
    return interface->att_node;    
}

/* API to return the neighbouring node of the interface provided in the argument*/
static inline node_t * get_nbr_node(interface_t *interface)
{

    assert(interface->att_node);
    assert(interface->link);
    
    link_t *link = interface->link;
    
    if(interface == &(link->intf1))
    {
      return link->intf2.att_node;
    }
    else
    {
      return link->intf1.att_node;
    }    
}	


/* API to get the position(index of the array) of the available slot in the node to attach an interface */
static inline int get_node_intf_available_slot(node_t *node)
{
    for(int i=0; i<MAX_INTF_PER_NODE; i++)
    {
       if (node->intf[i])
         continue;
       return i;
    }	 
    
    return -1;   
}	

/*API to return the pointer to the local interface of a node, by its interface name*/
static inline interface_t *get_node_if_by_name(node_t *node, char *if_name)
{
    interface_t *intf;
    for(int i=0; i<MAX_INTF_PER_NODE; i++)
    {
        intf = node->intf[i];
        if(!intf) return NULL;
        if(strncmp(intf->if_name, if_name, IF_NAME_SIZE) == 0)
        {
            return intf;
        }
    }

    return NULL;
}

/*API to return the pointer to the node, searched by its name*/
static inline node_t *get_node_by_node_name(graph_t *topo, char *node_name)
{
    node_t *node;
    glthread_t *curr;
    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr)
    {
        node = graph_glue_to_node(curr);
        if(strncmp(node->node_name, node_name, NODE_NAME_SIZE)==0)
        {
            return node;
        }

    }ITERATE_GLTHREAD_END(&topo->node_list, curr);

    return NULL;
}


/*Display Routines*/
void dump_graph(graph_t *graph);
void dump_node(node_t *node);
void dump_interface(interface_t *interface);


#endif /* __NW_GRAPH_ */

