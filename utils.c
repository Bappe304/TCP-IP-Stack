#include "utils.h"
#include <stdint.h>
#include <arpa/inet.h> // For inet_pton and inet_ntop

void apply_mask(char *prefix, char mask, char *str_prefix)
{
    uint32_t subnet_mask = 0xFFFFFFFF;
    uint32_t prefix_binary_form = 0;
    uint32_t mask_length = (uint32_t)(mask);

    /*Here the mask_length wouldn't accept any length bigger 
    than 32*/
    if(mask_length == 32)
    {
        strcpy(str_prefix,prefix,16);
        str_prefix[15] = '\0';
        return;
    }


    subnet_mask = subnet_mask << (32-mask_length);

    /*This basically converts the IPaddress from text to binary and converts it the prefix_binary_form to big-endian*/
    if(inet_pton(AF_INET, prefix, &prefix_binary_form) != 1)
    {
        printf("Invalid IP address format\n");
        return;
    }

    prefix_binary_form = ntohl(prefix_binary_form);
    /*Converts the prefix_binary_form from big-endian to host order byte*/

    prefix_binary_form = subnet_mask & prefix_binary_form;
    
    prefix_binary_form = htonl(prefix_binary_form);
    /*Converts the prefix_binary_form back to big-endian form*/
    /* Here we have converted again to big endian bcoz 
    inet_ntop() expects prefix_binary_form in big-endian*/

    inet_ntop(AF_INET, &prefix_binary_form, str_prefix,16);
    str_prefix[15] = '\0';

    /* IMP POINT---> Here it doesnt matter whether we use htonl() or ntohl() because both of these function
                     don't actually know the endianess of the input to them, they just know the byte order of 
                     the host machine and the network byte order, if the input's endianess doesn't match is just
                     reverses the input's byter order*/



}

void
layer2_fill_with_broadcast_mac(char *mac_array)
{
    /*We can't just copy the 0xFFFFFFFFFFFF into it as 'char arrays' are typically
    set byte wise and therefore this string would overflow while assigning.*/
    for(int i=0; i<6; i++)
    {
        mac_array[i] = 0xFF;
    }
}

#define IS_MAC_BROADCAST_ADDR(mac) \
        ((mac[0]==0xFF) && (mac[0]==0xFF) && (mac[0]==0xFF) && \
        (mac[0]==0xFF) && (mac[0]==0xFF) && (mac[0]==0xFF)) 

