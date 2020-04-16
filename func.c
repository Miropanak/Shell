#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

//funkcia skontroluje spravnost zadaneho portu
int check_port(char * port_num)
{
        int port = atoi(port_num);
        if(port >= 0 && port <= 65535)
                return 0;
        else
                return -1;
}

char * check_sock_path();

int check_args(int argc, char **argv, int * port, char * sock_path)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(argv[i], "-h") == 0)
                        help_msg();
                else if(strcmp(argv[i], "-p") == 0){
                        if(i+1 < argc && check_port(argv[i+1]) == 0){
                                *port = atoi(argv[i+1]);
                                i++;
                        }
                        else{
                                perror("check_port()");
                                return -1;
                        }
                }
                /*
                else if(strcmp(argv[i], "-u") == 0){
                        if(i+1 < argc ){
                                *port = argv[i+1];
                                i++;
                        }
                }*/
                else{
                        perror("func check_args()");
                        return -1;
                }
        }
        return 0;
}
