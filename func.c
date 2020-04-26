#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "func.h"

//error message function
void error(char * msg)
{
        printf("Error: %s\n", msg);
}

//get file name without spaces
char *get_file_name(char * name)
{
    char *new_name = malloc(20*sizeof(char));
    if(strlen(name) > 20)
        return NULL;
    int index = 0, i;
    for(i = 0; i < 19; i++){
        if(name[i] != ' '){
            new_name[index++] = name[i];
        }
    }
    new_name[index] = '\0';
    return new_name;
}

//check for sigh '<'
bool redirect_input(char * line)
{
    int i;
    for(i = 0; i < strlen(line); i++)
        if(line[i] == '<')
            return true;
    return false;
}

//check for sigh '>'
bool redirect_output(char * line)
{
    int i;
    for(i = 0; i < strlen(line); i++)
        if(line[i] == '>')
            return true;
    return false;
}

//finding pipes in command
bool found_pipes(char * line)
{
    int i;
    for(i = 0; i < strlen(line); i++)
        if(line[i] == '|')
            return true;
    return false;
}

//counting pipes in command
int count_pipes(char * command)
{
    int i, counter = 0;
    for(i = 0; i < strlen(command); i++){
        if(command[i] == '|')
            counter++;
    }
    return counter;
}

//check for builtin command
bool check_builtin(char * command)
{       

        if(strcmp(command, "help") == 0)
                return true;
        else if(strcmp(command, "quit") == 0)
                return true;
        else if(strcmp(command, "halt") == 0)
                return true;
        else return false;
}

//check if inserted is valid number composed only from numbers 
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

//check if port number is number between 1024 and 65535
bool check_port(char * port_num)
{       
        if(is_number(port_num)){
                int port = atoi(port_num);
                if(port >= 1024 && port <= 65535)
                        return true;
                else
                        return false;
        }
        return false;      
}

//check if socket path is not too long
bool check_sock_path(char * sock_path)
{
        if(strlen(sock_path) > 100){
                error("Socket path length exceeded");
                return false;
        }
        return true;        

}

//function for findinf specific option [-ciudph] in arguments
bool find_arg(int argc, char ** args, char *arg)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(args[i], arg) == 0)
                        return true;
        }
        return false;
}

 //checking for OPTIONS [-ciudp] and arguments IP_address, port_number and socket_path
int check_args(int argc, char **argv, int * port, char * sock_path, char * IP_addr, int * mode)
{
        int i;
        for(i = 1; i < argc; i++){
                if(strcmp(argv[i], "-h") == 0)
                        help_msg();
                else if(strcmp(argv[i], "-p") == 0){
                        //check if after options -p is valid port number
                        if(i+1 < argc && check_port(argv[i+1])){
                                *port = atoi(argv[++i]);
                                if(*mode != 1 && *mode != 2 && !find_arg(argc, argv, "-d"))
                                        *mode = 4;
                                else if(*mode != 1 && *mode != 2 && find_arg(argc, argv, "-d"))
                                        *mode = 6;
				continue;
                        }
                        else{
                                error("Invalid port Number");
                                exit(1);
                        }
                }
                else if(strcmp(argv[i], "-u") == 0){
                        //check if after options -u is valid socket path
                        if(i+1 < argc && check_sock_path(sock_path)){
                                strcpy(sock_path, argv[++i]);
                                //check if options -d is in arguments
                                if(*mode != 1 && *mode != 2 && !find_arg(argc, argv, "-d"))
                                        *mode = 3;
                                else if(*mode != 1 && *mode != 2 && find_arg(argc, argv, "-d"))
                                        *mode = 5;
                                continue;
                        }
                        else{
                                error("Invalid socket path");
                                exit(1);
                        }
                }
                //check if after options -i is valid IPv4 address
                else if(strcmp(argv[i], "-i") == 0){
                        if(i+1 < argc && strlen(argv[i+1]) < 16){
                                strcpy(IP_addr, argv[++i]);
                                continue;
                        }
                        else{
                                error("Invalid IP adress");
                                exit(1);
                        }
                }
                //check if it will be unix client or internet client
                else if((strcmp(argv[i], "-c") == 0) && (find_arg(argc, argv, "-p") || find_arg(argc, argv, "-u"))){
                        if(find_arg(argc, argv, "-u"))
                                *mode = 1;
                        else
                                *mode = 2;	
                }
                //check for builtin command help
                else if(((strcmp(argv[i], "-help") == 0) && find_arg(argc, argv, "-c")) || ((strcmp(argv[i], "-c") == 0) && find_arg(argc, argv, "-help"))){
                        printf("%s", help_msg());
                        return -1;       
                }
                //check for builtin command halt
                else if(((strcmp(argv[i], "-halt") == 0) && find_arg(argc, argv, "-c")) || ((strcmp(argv[i], "-c") == 0) && find_arg(argc, argv, "-halt"))){
                        printf("Closing shell...\n");
                        return -1;
                }
                //check for builtin command quit
                else if(((strcmp(argv[i], "-quit") == 0) && find_arg(argc, argv, "-c")) || ((strcmp(argv[i], "-c") == 0) && find_arg(argc, argv, "-quit"))){
                        printf("There is no connection to quit.\n");
                        return -1;
                }
                //check for option -d for creating deamon
		else if((strcmp(argv[i], "-d") == 0)){
                        create_deamon();
                        //check if options -u or -i
                        if(!find_arg(argc, argv, "-u") && !find_arg(argc, argv, "-i")){
                                exit(1);
                        }
                }
                //invalid arguments
                else{
			error("Invalid Arguments\n");
                       	return -1; 
                }
        }
        return 0;
}
