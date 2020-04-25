#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void help_msg()
{
        printf("\t\t\tSHELL\t\t\t\n");
        printf("----------------------------------------------------\n");
        printf("Author: Miroslav Pavlak\n");
        printf("NAME\n\tShell -- interactive shell\n");
        printf("SYNOPSIS\n");
        printf("\t./shell\n");
        printf("\t./shell [-uip] \n");
        printf("\t./shell -c [-u | ip] \n");
        printf("\t./shell -c [Builin commands]\n");
        printf("\t./shell -d [-c] [-u | ip]\n\n");

        printf("Builtin commands: \n");
        printf("\thelp\tshow manual page\n");
        printf("\tquit\tcancel connection with server shell\n");
        printf("\thalt\texit shell\n\n");
        printf("Options:\n");
        printf("\t-h\n\t\tshow manual page and start shell\n\n");
        printf("\t-c <builin_command>\n\t\tshell execute builtin command once then exits\n\n");
        printf("\t-c <-u/-i>\n\t\tshell became client, which can connect to another shell using switch -u <socket_path> or -i <IP_address> -p <port_number>\n\n");
        printf("\t-u <socket_path>\n\t\tspecific unix domain socket path for connection\n\n");
        printf("\t-i <IP_address>\n\t\tspecific IPv4 adress for shell connection\n\n");
        printf("\t-p <port_number>\n\t\tspecific port number for shell connection\n\n");
        printf("\t-d\n\t\tshell behave like deamon\n\n");
        printf("EXAMPLES: \n");
        printf("\t./shell -c -help\t\n");
        printf("\t./shell -c -halt\t\n");
        printf("\t./shell -u ./sck\t\n");
        printf("\t./shell -c -u ./sck\t\n");
        printf("\t./shell -p 54321 \t\n");
        printf("\t./shell -i 147.175.99.100 -p 54321\t\n");
        printf("\t./shell -c -t 147.175.99.100 -p 54321\t\n");
        printf("\t./shell -d -u ./sck\t\n");
        printf("\t./shell -d -i 147.175.99.100 -p 54321\t\n\n");
}

void help()
{
        help_msg();
}

void create_deamon()
{
        pid_t pid, sid;
        pid = fork();
        //check if fork is sucessfull
        if(pid < 0){
                perror("fork()");
                exit(1);
        }
        //terminating parent
        if(pid > 0){
                exit(0);
        }
        //deamon created
        umask(0);
        sid = setsid();
        if(sid < 0){
                perror("setsid()");
                exit(1); 
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
}

