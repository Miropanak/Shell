#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include "func.h"

void unix_client(char * sock_path)
{
        char server_msg[4096], *buff;
        int sock, read_bytes, running = 1;
        struct sockaddr_un un_ad;

        memset(&un_ad, '\0', sizeof(un_ad));
        un_ad.sun_family = AF_UNIX;
        strncpy(un_ad.sun_path, sock_path, 20);
        //create unix socket
        if((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
                perror("socket()");
                exit(2);
        }
        //connecting to server socket
        if((connect(sock, (struct sockaddr*)&un_ad, sizeof(un_ad))) < 0){
                perror("socket()");
                exit(2); 
        }
        printf("Remote@~");
        //main loop
        while(running){
                read_bytes = read(sock, server_msg, sizeof(server_msg));        //read message from server
                server_msg[read_bytes] = '\0';
                //closing connection       
		if(strcmp(server_msg, "Closing connection...\n") == 0){              
			running = 0;
			break;
		}
                printf("%s", server_msg);
               	do{
			buff = readline(">>> ");                //read command from stdin
		}while(strlen(buff) == 0);
                memset(server_msg, '\0', sizeof(server_msg));   
		write(sock, buff, strlen(buff));                //send command to server        
        }
        close(sock);
}

void internet_client(int port, char * IP_addr)
{       
        char server_msg[4096], *buff;
        int sock, read_bytes, running = 1;
        struct sockaddr_in in_ad;

        bzero((char*)&in_ad, sizeof(in_ad));
        in_ad.sin_family = AF_INET;
        in_ad.sin_port = htons(port);
        //check if IP_addr is emty
        if(strlen(IP_addr) == 0)
                in_ad.sin_addr.s_addr = inet_addr("127.0.0.1");         //default IP_address
        else
                in_ad.sin_addr.s_addr = inet_addr(IP_addr);             //custom IP address
        
        //create internet socket
        if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
                perror("socket()");
                exit(2);
        }

        //connect to server socket
        if((connect(sock, (struct sockaddr*)&in_ad, sizeof(in_ad))) < 0){
                perror("socket()");
                exit(2); 
        }
        printf("Remote@~");
        while(running){
                read_bytes = read(sock, server_msg, sizeof(server_msg));        //read message from server                
                server_msg[read_bytes] = '\0';
		 //closing connection       
                if(strcmp(server_msg, "Closing connection...\n") == 0){
			running = 0;
			break;
		}
               	printf("%s", server_msg);
               	do{
			buff = readline(">>> ");                //read command from stdin
		}while(strlen(buff) == 0);
                memset(server_msg, '\0', sizeof(server_msg));  
		write(sock, buff, strlen(buff));                //send command to server 
        }
        close(sock);
}
