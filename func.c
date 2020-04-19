#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

void error(char * msg)
{
        printf("Error: %s\n", msg);
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
        /*sem asi este dpolnit skontrolovanie ci existuje a ak neexistuje
        tak ho treba vytvorit a dalej vo funkcii kde budem nastavoval socket*/
        return true;        

}

int check_args(int argc, char **argv, int * port, char * sock_path)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(argv[i], "-h") == 0)
                        help_msg();
                else if(strcmp(argv[i], "-p") == 0){
                        if(i+1 < argc && check_port(argv[i+1])){
                                *port = atoi(argv[i+1]);
                                i++;
                        }
                        else{
                                error("Invalid port Number");
                                return -1;
                        }
                }
                else if(strcmp(argv[i], "-u") == 0){
                        if(i+1 < argc && check_sock_path(sock_path)){
                                strcpy(argv[i+1], sock_path);
                                return 0;
                        }
                        else{
                                error("Invalid socket path");
                                return -1;
                        }
                }
                else{
                       return -1; 
                }
                
        }
        return 0;
}
