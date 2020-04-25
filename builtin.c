#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void help_msg()
{
        printf("\t\t\t\t\t  SHELL  \t\t\t\t\t\n"
        "----------------------------------------------------------------------------------------\n"
        "Author: Miroslav Pavlak\n\n"
        "NAME\n\tShell -- interactive shell\n\n"
        "SYNOPSIS\n"
        "\t./shell\n"
        "\t./shell [-chd] [-uip]\n"
        "\t./shell -c [BUILTINS]\n"
        "\t./shell [-u <socket_path> | -i <IP_address> -p <port_number>] \n"
        "\t./shell -c [-u <socket_path> | -i <IP_address> -p <port_number>] \n"
        "\t./shell -d [-u <socket_path> | -i <IP_address> -p <port_number>]\n\n");

        printf("DESCRIPTION:\n"
        "\tShell is standart command interpreter compatible with OS FreeBSD 5.2.1. Shell\n"
        "\tallows execute commands from STDIN, Unix socket or Internet socket. Option -c\n"
        "\t[-uip] allows shell connects to another shell and then execute commands. Option\n"
        "\t-d [-uip] makes from normal shell deamon, waiting for connection and executes\n"
        "\tdemanded commands.\n\n"
        
        "\tArgument socket_path should be unix domain socket through which communication\n"
        "\twill be perfomed. And IP_address should be valid address of listening shell.\n"
        "\tPort_number should be number of free port between 1024 - 65535.\n\n");
        
        printf("BUILTINS: \n"
        "\thelp\tshow manual page\n"
        "\tquit\tcancel connection with server shell\n"
        "\thalt\texit shell\n\n");

        printf("OPTIONS:\n"
        "\t-c [BUILTINS]\n\t\texecute builtin command once then exit\n\n"
        "\t-c [-u|ip]\n\t\tmake connection to another shell\n\n"
        "\t-d [-u|ip]\n\t\tshell behave like deamon, and wait for connection\n\n"
        "\t-h\n\t\tshow manual page and start shell\n\n"
        "\t-i <IP_address>\n\t\tspecific IPv4 adress for shell connection\n\n"
        "\t-p <port_number>\n\t\tspecific port number for shell connection\n\n"
        "\t-u <socket_path>\n\t\tspecific unix domain socket path for connection\n\n");

        printf("ARGUMENTS\n"
        "\tsocket_path\n\t\tunix domain socket path for connection\n"
        "\tIP_address\n\t\tIP address, where Shell waits for connection\n"
        "\tport_number\n\t\tport number, where Shell waits for connection\n\n");

        printf("EXAMPLES:\n"
        "\t./shell -c -help\t\n"
        "\t./shell -u ./sck\t\n"
        "\t./shell -c -u ./sck\t\n"
        "\t./shell -p 54321 \t\n"
        "\t./shell -i 147.175.99.100 -p 54321\t\n"
        "\t./shell -c -i 147.175.99.100 -p 54321\t\n"
        "\t./shell -d -u ./sck\t\n"
        "\t./shell -d -i 147.175.99.100 -p 54321\t\n\n");

        printf("ERRORS:\n"
        "\tBUILTINS do not support in/out redirection or piping commands\n");
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

