#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "func.h"

#define MAX_COMMANS_LEN 128
#define MAX_NUM_OF_COMMANDS 20

char ** pars_args(char * line, char * separator, int *argc)
{
	int index = 0;
	char * command;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));

	command = strtok(line, separator);
	while(command != NULL){
		commands[index] = command;
		index++;
		command = strtok(NULL, separator);
	}
	commands[index] = NULL;
	*argc = index;
	return commands;
}

char * cut_comment(char * line)
{
	char * new_line = calloc(127, sizeof(char));
	int i;
	for(i = 0; i < 127; i++){
		if(line[i] != '#' && line[i] != '\\')
			new_line[i] = line[i];
		else
			break;
	}
	new_line[i] = '\0';
	printf("dlzka prikazu %s %d\n", new_line, strlen(new_line));
	return new_line;
}

int execute_builtin(char * command)
{
	if(strcmp(command, "help") == 0)
		help_msg();
	else if(strcmp(command, "quit") == 0)
		quit();
	else if(strcmp(command, "halt") == 0)
		return 0;
	else{
		error("Invalid builtin command\n");
		return 0;
	}
	return 1;
}

int exec_cmd(char * command, int argc)
{	
	int num_of_args;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	pid_t child_pid;
	commands = pars_args(command, " ", &num_of_args);
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
	return 1;
}

exec_cmd_

int execute_commands(char ** command, int argc)
{
	int ret_val;
	if(argc < 1)
			return 0;
	else if(argc == 1){
		if(check_builtin(command[0])){
			ret_val = execute_builtin(command[0]);
			return ret_val;
		}
		else{
			ret_val = exec_cmd(command[0], argc);	
		}
	}
	else if(argc > 1){

	}
	return 1;
}

void shell_loop()
{
	char * usr_input;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	char ** split_cmd = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	char ** atomic_cmd = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	int argc, split_argc, atom_argc, i;
	int state = 1;
	//bool run_flag = true;
	//pid_t child_pid;

	while(state){
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
		//parsovanie podla \ a #
		else{
			//tu musi byt cyklus na vykonanie vsetkych prikazov
			usr_input = cut_comment(usr_input);
			commands = pars_args(usr_input, ";", &argc);                    //parsovanie prikazov podla ';'
			//kazdy zlozeny prikaz v cykle sa rozdeli podla |,<,>
			for(i = 0; i < argc; i++){
				split_cmd = pars_args(commands[i], "|<>", &split_argc);
				print_args(split_cmd, split_argc);
				state = execute_commands(split_cmd, split_argc);
			}
		}
		free(usr_input);
	}
}