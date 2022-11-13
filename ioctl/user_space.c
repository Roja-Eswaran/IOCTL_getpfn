#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "kernel_module.h"

void get_pfn(int fd, unsigned long vpn_in)
{
        query_arg_t q;
        q.vpn = vpn_in;

        if (ioctl(fd, IOCTL_GET_PFN, &q) == -1)
        {
                perror("user_space ioctl get pfn");
        }
        else
        {
                if(q.pfn)
                        printf("VPN = %lx, PFN = %16lx\n", q.vpn, q.pfn);
                else
                        printf("No vaild\n");
        }
}

int main(int argc, char *argv[])
{
        char *file_name = "/dev/query";
        int fd;
        unsigned long vpn;
        if (argc == 2)
        {
                vpn = strtoul(argv[1], NULL, 16);
        }else
        {
                fprintf(stderr, "Usage: %s [virtual page number]\n", argv[0]);
                return 1;
        }
        fd = open(file_name, O_RDWR);
        if (fd == -1)
        {
                perror("query_apps open");
                return 2;
        }
        //printf("vpn_in = %lx\n", vpn); 
        get_pfn(fd, vpn);

        close (fd);

        return 0;
}
