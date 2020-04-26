#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "func.h"

int main(int argc, char **argv)
{       
        //mode 0 normal mode
        //mode 1 unix client
        //mode 2 inet client
        //mode 3 unix server
        //mode 4 inet server
        //mode 5 unix server deamon
        //mode 6 inet server deamon

        int port = 0, mode = 0;
        char * sock_path = calloc(20, sizeof(char));
        char * IP_addr = calloc(16, sizeof(char));

        //checking for OPTIONS [-ciudph] and arguments IP_address, port_number and socket_path
        if(check_args(argc, argv, &port, sock_path, IP_addr, &mode) == -1){
                 return 0;
        }

        //if shell is in client mode 1 or 2
        if(mode == 1 || mode == 2){
                if(mode == 1)
                        unix_client(sock_path);                    //start unix client
                else
                        internet_client(port, IP_addr);            //start              

                free(sock_path);        //free memory                                                    
                free(IP_addr);          //free memory                
                printf("Remote shell closed\n");
                return 0;
        }

        //main shell loop
        shell_loop(port, sock_path, IP_addr, mode);

        return 0;
}
