#include <stdio.h>
#include <string.h>
#include "func.h"

int main(int argc, char **argv)
{
        int port = 0;
        char *sock_path;

        if(check_args(argc, argv, &port, sock_path) == -1){
                perror("check_args()");
                return -1;
        }

        shell_loop();

        return 0;
}