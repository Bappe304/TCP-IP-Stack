#ifndef __NET__
#define __NET__

#include "utils.h"
#include <memory.h>

/* Device IDS */
#define L3_ROUTER (1 << 0)
#define L2_SWITCH (1 << 1)
#define HUB       (1 << 2)


typedef struct graph_ graph_t;
typedef struct interface_ interface_t;
typedef struct node_ node_t;

typedef struct ip_add_
{
    char ip_addr[16];
} ip_add_t;

typedef struct mac_add_
{
    char mac[6];
} mac_add_t;

typedef struct nope_nw_prop_
{
    unsigned int flags;

    /* L3 properties */
    bool_t is_lb_addr_config;
    ip_add_t lb_addr;  /*lookback address basically used to testing by pinging the node itself*/
} node_nw_prop_t;

static inline void 
init_node_nw_prop(node_nw_prop_t *node_nw_prop)
{
    node_nw_prop->flags = 0;
    node_nw_prop->is_lb_addr_config = false;
    memset(node_nw_prop->lb_addr.ip_addr, 0, 16);
}

typedef struct intf_nw_props_
{
    /*L2 properties*/
    mac_add_t mac_add; /*MAC address is hardcoded in the NIC by the manufacturer*/

    /*L3 properties*/
    bool_t is_ipadd_config; /*This should be set to TRUE incase the interface 
    has an IP address which also means that it would operate in the L3 mode*/

    ip_add_t ip_add;
    char mask;
} intf_nw_props_t;

static inline void
init_intf_nw_prop(intf_nw_props_t *intf_nw_props)
{
    memset(intf_nw_props->mac_add.mac, 0, 48);
    intf_nw_props->is_ipadd_config = false;
    memset(intf_nw_props->ip_add.ip_addr, 0, 16);
    intf_nw_props->mask = 0;
}

void 
interface_assign_mac_address(interface_t *interface);

/*Macros for getting the IP and MAC addresses*/
#define IF_MAC(intf_ptr)  ((intf_ptr)->intf_nw_props.mac_add.mac)
#define IF_IP(intf_ptr)   ((intf_ptr)->intf_nw_props.ip_add.ip_addr)

/*IP for Node*/
#define NODE_LO_ADDR(node_ptr)  ((node_ptr)->node_nw_prop.lb_addr.ip_addr)



/* API declaration here*/
/* APIs to set the Network Node properties and Interface network properties*/
bool_t node_set_device_type(node_t *node, unsigned int F);
bool_t node_set_loopback_address(node_t *node, char *ip_addr);
bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask);
