#include "utils.h"
#include <stdint.h>
#include <arpa/inet.h> // For inet_pton and inet_ntop

void apply_mask(char *prefix, char mask, char *str_prefix)
{
    uint32_t subnet_mask = 0xFFFFFFFF;
    uint32_t prefix_binary_form = mask;
    uint32_t mask_length = (uint32_t)(mask);

    if(mask_length == 32)
    {
        strcpy(str_prefix,prefix,16);
        str_prefix[15] = '\0';
        return;
    }

    subnet_mask = subnet_mask << (32-mask_length);

    if(inet_pton(AF_INET, prefix, &prefix_binary_form) != 1)
    {
        printf("Invalid IP address format\n");
        return;
    }

    
}