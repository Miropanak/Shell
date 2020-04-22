#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"

int main(int argc, char **argv)
{
        int port = 0, client = 0;
        char *sock_path = calloc(20, sizeof(char));

        if(check_args(argc, argv, &port, sock_path, &client) == -1){
                 return 0;
        }

        if(client){
                //tu treba nakodit pripojenie sa na socket a v loope
                //budem posielat prikazy na socket
                //bud unixovy/internetovy
                printf("spravam sa ako klient\n");
                return 0;
        }

        shell_loop(port, sock_path);

        return 0;
}

/*int main(int argc, char **argv)
{
	int port = 0, client;
	char *sock_path, ;

	if(check_args(argc, argv, &port, sock_path) == -1){
		error("check_args()");
		return -1;
	}

	shell_loop();

        return 0;
}*/
