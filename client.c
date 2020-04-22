#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <sys/un.h>
#include <string.h>

int main(int ac, char **av, char **en) {
        int s, r, running = 1;
        char msg[64], *buf;
        struct sockaddr_un ad;  // adresa pre soket

        memset(&ad, 0, sizeof(ad));
        ad.sun_family = AF_LOCAL;
        strcpy(ad.sun_path, av[1]);   // adresa = meno soketu (rovnake ako pouziva server)
        s = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (s == -1)
        {
                perror("socket: ");
                exit(2);
        }

        printf("running as client\n");
        connect(s, (struct sockaddr*)&ad, sizeof(ad));  // pripojenie na server
        while(running){
                r = read(s, msg, sizeof(msg));
                msg[r] = '\0';
		if(strcmp(msg, "Closing connection...\n") == 0){
			running = 0;
			break;
		}
               	do{	
			printf("Remote@~%s", msg);
			buf = readline("");
		}while(strlen(buf) == 0);
		write(s, buf, strlen(buf));     // poslanie spravy
        }
        close(s);
        return 0;
}
