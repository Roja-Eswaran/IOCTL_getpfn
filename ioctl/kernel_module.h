#ifndef KERNEL_MODULE
#define KERNEL_MODULE
#include <linux/ioctl.h>

typedef struct
{
        unsigned long vpn, pfn;
        }query_arg_t;

        #define IOCTL_GET_PFN _IOWR('q', 1, query_arg_t *)
        #endif
