#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

void error(char * msg)
{
        printf("Error: %s\n", msg);
}

bool check_builtin(char * command)
{
        if(strcmp(command, "help") == 0)
                return true;
        else if(strcmp(command, "quit") == 0)
                return true
        else if(strcmp(command, "halt") == 0)
                return true;
        else return false;
}

bool is_number(char * num)
{
        int i;
        for(i = 0; i < strlen(num); i++){
           if(num[i] < 48 || num[i] > 57){
                error("Entered port is not number value\n");
                return false;
           }
        }
        return true;
}

//funkcia skontroluje spravnost zadaneho portu
bool check_port(char * port_num)
{       
        if(is_number(port_num)){
                int port = atoi(port_num);
                if(port >= 0 && port <= 65535)
                        return true;
                else
                        return false;
        }
        return false;      
}

bool check_sock_path(char * sock_path)
{
        if(strlen(sock_path) > 100){
                error("Socket path length exceeded");
                return false;
        }
        return true;        

}

//funkcia zisti vyskyt prislusneho prepinaca v poli argumnetov
bool find_arg(int argc, char ** args, char *arg)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(args[i], arg) == 0)
                        return true;
        }
        return false;
}

int check_args(int argc, char **argv, int * port, char * sock_path, int * client)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(argv[i], "-h") == 0)
                        help_msg();
                else if(strcmp(argv[i], "-p") == 0){
                        if(i+1 < argc && check_port(argv[i+1])){
                                *port = atoi(argv[i+1]);
                                i++;
                                continue;
                        }
                        else{
                                error("Invalid port Number");
                                return -1;
                        }
                }
                else if(strcmp(argv[i], "-u") == 0){
                        if(i+1 < argc && check_sock_path(sock_path)){
                                strcpy(sock_path, argv[++i]);
                                continue;
                        }
                        else{
                                error("Invalid socket path");
                                return -1;
                        }
                }
                else if(strcmp(argv[i], "-c") == 0 && (find_arg(argc, argv, "-p") || find_arg(argc, argv, "-u"))){
                        *client = 1;
                        printf("tusom\n");
                }

                else if(((strcmp(argv[i], "-help") == 0) && find_arg(argc, argv, "-c")) || ((strcmp(argv[i], "-c") == 0) && find_arg(argc, argv, "-help"))){
                        help_msg();
                        return -1;
                }
                else if(((strcmp(argv[i], "-halt") == 0) && find_arg(argc, argv, "-c")) || ((strcmp(argv[i], "-c") == 0) && find_arg(argc, argv, "-halt"))){
                        printf("Closing shell...\n");
                        return -1;
                }
                 else if((strcmp(argv[i], "-d") == 0)){
                        printf("I am deamon");
                        close(STDIN_FILENO);
                        close(STDOUT_FILENO);
                        close(STDERR_FILENO);
                        sleep(10);
                        return -1;
                }
                else{
                        error("Invalid Arguments\n");
                        return -1;
                }

        }
        return 0;
}
