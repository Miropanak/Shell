#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int stdout_copy;

void help_msg()
{
        printf("\t\t\tSHELL\t\t\t\n");
        printf("----------------------------------------------------\n");
        printf("Autor: Miroslav Pavlak\n");
        printf("Ucel: \n");
        printf("Pouzitie: \n");
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
        stdout_copy = dup(1);
        printf("create deamon %d\n", getpid());
        close(STDIN_FILENO);
        //close(STDOUT_FILENO);
        close(STDERR_FILENO);
}

