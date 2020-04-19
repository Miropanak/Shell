#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "func.h"

#define MAX_COMMANS_LEN 128
#define MAX_NUM_OF_COMMANDS 20

char ** pars_args(char * line)
{
	int i, index = 0;
	char * command;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));

	command = strtok(line, " ");
	while(command != NULL){
		commands[index] = command;
		index++;
		command = strtok(NULL, " ");
	}
	commands[index] = NULL;
	for(i = 0; i < index; i++)
		printf("commands -> %s\n", commands[i]);
	
	return commands;
}

int execute(char ** commands);

void shell_loop()
{
	char * usr_input;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	int status;
	pid_t child_pid;
	
	while(true){
		//precitanie riadku
		usr_input = readline(print_prompt());
		//stlaceny enter
		if(strlen(usr_input) == 0){
			continue;
		}
		//prekrocena dlzka prikazu
		else if(strlen(usr_input) > 128){
			error("MAX_COMMANS_LEN exceeded");
			return;
		}
		//parsing to args
		else{
			commands = pars_args(usr_input);
			child_pid = fork();
			if(child_pid < 0){
				perror("fork()");	
			}
			else if(child_pid == 0){
				if(execvp(commands[0], commands) < 0)
					exit(EXIT_FAILURE);
			}
			else{
				waitpid(child_pid, &status, WUNTRACED);
			}
		}
	}
}