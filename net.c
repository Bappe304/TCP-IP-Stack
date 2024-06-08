/* Description----> This file contains general purpose Networking routines and APIs*/
#include "graph.h"
#include "net.h"
#include "utils.h"
#include <memory.h>
#include <utils.h>
#include <stdio.h>



static unsigned int 
hash_code(void *ptr, unsigned int size)
{
    unsigned int value=0, i=0;
    char *str = (char*)ptr;
    while(i < size)
    {
        value += *str;
        value *= 97;
        str++;
        i++;
    }
    return value;
}


void
interface_assign_mac_address(interface_t *interface)
{
    node_t *node = interface->att_node;

    if(!node)
        return;

    unsigned int hash_code_val = 0;
    hash_code_val = hash_code(node->node_name, NODE_NAME_SIZE);
    hash_code_val *= hash_code(interface->if_name, IF_NAME_SIZE);
    memset(IF_MAC(interface), 0, sizeof(IF_MAC(interface)));
    memcpy(IF_MAC(interface), (char*)&hash_code_val, sizeof(unsigned int));
}

/*This is basically setting the device type of the node whether it is a 
ROUTER, SWITCH, HUB*/
bool_t node_set_device_type(node_t *node, unsigned int F)
{
    SET_BIT(node->node_nw_prop.flags, F);
    return TRUE;
}

bool_t node_set_loopback_address(node_t *node, char *ip_addr)
{
    assert(ip_addr);

    if(IS_BIT_SET(node->node_nw_prop.flags, HUB))
        assert(0); /*Wrong config: A HUB does not have any IP Address*/
    
    if(!IS_BIT_SET(node->node_nw_prop.flags, L3_ROUTER))
        assert(0); /*Need to enable L3 routing on the device first*/

    node->node_nw_prop.is_lb_addr_config = TRUE;
    strncpy(NODE_LO_ADDR(node), ip_addr, 16);
    NODE_LO_ADDR(node)[15] = '\0';

    return TRUE;
}

bool_t node_set_intf_ip_address(node_t *node, char *local_if,char *ip_addr, char mask)
{
    interface_t *interface = get_node_if_by_name(node, local_if);
    if(!interface) assert(0);

    interface->intf_nw_props.is_ipadd_config = TRUE;
    strncpy(IF_IP(interface), ip_addr, 16);
    IF_IP(interface)[15] = '\0';
    interface->intf_nw_props.mask = mask;
    return TRUE;
}


/*This unset API isnt being used in the scope of this 
but still we have defined it for future purpose*/

bool_t node_unset_intf_ip_address(node_t *node, char *local_if)
{
    return TRUE;
}

void dump_node_nw_props(node_t *node)
{
    printf("\nNode Name = %s\n", node->node_name);
    printf("\t node flags : %u", node->node_nw_prop.flags);
    if(node->node_nw_prop.is_lb_addr_config)
    {
        printf("\t IP loopback addr : %s/32\n", NODE_LO_ADDR(node));
    }
}

void dump_intf_nw_props(interface_t *interface)
{
    dump_interface(interface);

    if(interface->intf_nw_props.is_ipadd_config)
    {
        printf("\t IP Addr = %s/%u", IF_IP(interface), interface->intf_nw_props.mask);
        /*Here we are displaying both the IP address and the mask for it*/
    }
    else
    {
        printf("\t IP Addr = %s/%u", "Nil", 0);
    }

    printf("\t MAC : %u:%u:%u:%u:%u:%u\n", 
        IF_MAC(interface)[0],IF_MAC(interface)[1],
        IF_MAC(interface)[2],IF_MAC(interface)[3],
        IF_MAC(interface)[4],IF_MAC(interface)[5]);
}

void dump_nw_graph(graph_t *graph)
{
    node_t *node;
    glthread_t *curr;
    interface_t *interface;
    unsigned int i;

    printf("Topology Name = %s\n", graph->topology_name);

    ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr)
    {
        node = graph_glue_to_node(curr);
        dump_node_nw_props(node);
        for(i=0; i<MAX_INTF_PER_NODE; i++)
        {
            interface = node->intf[i];
            if(!interface) break;
            dump_intf_nw_props(interface);
        }
    }
    ITERATE_GLTHREAD_END(&graph->node_list, curr);
}