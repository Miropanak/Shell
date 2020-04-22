#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "func.h"

int main(int argc, char **argv)
{
        int port = 0, client = 0;
        char *sock_path = calloc(20, sizeof(char));

        if(check_args(argc, argv, &port, sock_path, &client) == -1){
                 return 0;
        }

        if(client){
                if(port == 0){
                        unix_client(sock_path);
                }
                else{
                        internet_client(port);
                }

                free(sock_path);
                port = 0;
                printf("Remote shell closed %d\n", getpid());
        }

        shell_loop(port, sock_path);

        return 0;
}
