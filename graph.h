#include "gluethread/glthread.h"
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

}interface_t;


struct link_{
    interface_t intf1;
    interface_t intf2;
    unsigned int cost;
};

struct node_{
    char node_name[NODE_NAME_SIZE];
    interface_t *intf[MAX_INTF_PER_NODE] = {NULL};
    glthread_t graph_glue;
}	

typedef struct graph_{
    
    char topology_name[32];
    glthread_t node_list;

}



/* API to return the node of the interface passed as in the argument */
static inline node_t* get_node(interface_t *interface)
{
    if(interface == NULL)
    {
	    return NULL;
    }
    
    return interface->att_node;    
}

/* API to return the neighbouring node of the interface provided in the argument*/
static inline node_t* get_nbr_node(interface_t *interface)
{
    link_t *link = interface->link;
    
    if(interface == &(link.intf1))
    {
      return intf2->att_node;
    }
    else
    {
      return intf1->att_node;
    }    
}	


/* API to get the position(index of the array) of the available slot in the node to attach an interface */
static inline int get_node_intf_available_slot(node_t *node)
{
    for(int i=0; i<MAX_INT_PER_NODE; i++)
    {
       if node->intf[i] == NULL;
       return i;
    }	 
    
    return -1;   
}	



