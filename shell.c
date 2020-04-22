#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "func.h"

int main(int argc, char **argv)
{       
        //mode 0 normal more
        //mode 1 unix client
        //mode 2 inet client
        //mode 3 unix server
        //mode 4 inet server
        int port = 0, mode = 0;
        char * sock_path = calloc(20, sizeof(char));
        char * IP_addr = calloc(16, sizeof(char));

        if(check_args(argc, argv, &port, sock_path, IP_addr, &mode) == -1){
                 return 0;
        }

        if(mode == 1 || mode == 2){
                if(mode == 1)
                        unix_client(sock_path); 
                else
                        internet_client(port, IP_addr);

                free(sock_path);
                free(IP_addr);
                port = 0;
                mode = 0;
                printf("Remote shell closed %d\n", getpid());
        }

        shell_loop(port, sock_path, IP_addr, mode);

        return 0;
}
