#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "func.h"

#define MAX_COMMANDS_LEN 128
#define MAX_NUM_OF_COMMANDS 20

//parsing arguments from line based on specific separators
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

//ignoring text after # sign
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
	return new_line;	
}

//execute builtin commands
int execute_builtin(char * command)
{
	if(strcmp(command, "help") == 0)
		write(1, help_msg(), 2048);
	else if(strcmp(command, "quit") == 0)
		printf("There is no connection to quit.\n");
	else if(strcmp(command, "halt") == 0)
		return 0;	
	else{
		error("Invalid builtin command\n");
		return 0;
	}
	return 1;		
}

//execute command
int exec_cmd(char * command, int argc)
{	
	int num_of_args, status;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	pid_t child_pid;
	commands = pars_args(command, " ", &num_of_args);			//parsing command
	
	child_pid = fork();					//create child process
	if(child_pid < 0){
		perror("fork()");
	}
	//execute command in child process
	else if(child_pid == 0){
		if(execvp(commands[0], commands) < 0){
			perror("execvp()");
			_Exit(EXIT_FAILURE);
		}		
	}
	//parent wait until child terminates
	else{
		waitpid(child_pid, &status, WUNTRACED);
	}
	return 1;
}

//execute command with pipes and output redirection
int exec_pipe_out(char ** command, int argc, bool out)
{
	int i, num_of_pipes, *pipe_fd, status, argc_num = argc, counter = 0, fd;
	pid_t child;
	char *file_name, name[20];
	//if there is output redirection
	if(out){
		num_of_pipes = argc - 2;
		file_name = malloc(20*sizeof(char));		//get file name for redirecting output
		//file name is too long		
		if(strlen(command[argc - 1]) > 19){
			error("Too long name of file");
			exit(1);
		}
		//file name is NULL
		if((file_name = get_file_name(command[argc_num -1])) == NULL)
			exit(1);
		//create/open file name form writing
		if((fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
			perror("open()");
			exit(1);
		}
	}	
	else
		num_of_pipes = argc - 1;

	//creating pipes if there is any
	if(num_of_pipes > 0){
		pipe_fd = malloc(num_of_pipes*2*sizeof(int));
		//create pipes
		for(i = 0; i < num_of_pipes; i++){
			if((pipe(pipe_fd + i*2)) < 0){
				perror("pipe()");
				exit(1);
			}   
		}
	}
    	
	//main loop for executing commands
    while((!out && counter < argc) || (out && counter < argc-1)){
        //create new child
        if((child = fork()) == 0){
			char ** cmd = (char **)malloc(MAX_COMMANDS_LEN*sizeof(char*));
			cmd = pars_args(command[counter], " ", &argc_num);
            //first command
            if(counter == 0){
				//if first command is last command
				if(out && argc == 2)
					dup2(fd, STDOUT_FILENO);
                else
					dup2(pipe_fd[1], STDOUT_FILENO);
            }
            //last command
            else if(counter == argc-1){
              	dup2(pipe_fd[counter*2-2], STDIN_FILENO);
            }
            //middle commands
            else{
				dup2(pipe_fd[counter*2-2], STDIN_FILENO);
				//if this is last command before redirecting output
				if(out && counter == argc-2){
					if(close(STDOUT_FILENO) < 0)
						perror("close(0)");
					dup(fd);
				}	
				else
                	dup2(pipe_fd[counter*2+1], STDOUT_FILENO);
            }
			//closing file descriptor of opened file
			if(out && counter == argc-2)
				if(close(fd) < 0)
					perror("close(0)");

			//closing created pipes
			for(i = 0; i < num_of_pipes*2; i++)
				if(close(pipe_fd[i]) < 0)
					perror("Close(1)");
			
			//executing command
            if(execvp(cmd[0], cmd) < 0)
                perror("execvp()");
            exit(1);
        }
        //fork error
        else if(child < 0){
            perror("fork()");
            exit(1);
        } 
        //parent increase counter  
        else{
            counter++;
        }
    }
	
	//closing pipes
	for(i = 0; i < num_of_pipes*2; i++)
        if(close(pipe_fd[i]) < 0)
			perror("close(3)");

	//wait for all children to terminates
    for (i = 0; i < counter; i++)
        wait(&status);
	
	//closing file descriptor of opened file
	if(out){
		if(close(fd) < 0)
			perror("close(2)");
		sprintf(name, "%s\n", file_name);
		write(1, name, strlen(name));
	}
	return 1;
}

//execute command input redirection
int exec_redirect_in(char ** command, int argc)
{
	int fd, status, argc_num;
	pid_t child;
	char *file_name = malloc(20*sizeof(char));
	//chech file name length
	if(strlen(command[argc-1]) > 19){
		error("Too long name of file");
		exit(1);
	}
	//file name is NULL
	if((file_name = get_file_name(command[argc-1])) == NULL)
		exit(1);
	//open file for reading
	if((fd = open(file_name, O_RDONLY, S_IRUSR | S_IWUSR)) < 0){
		perror("open()");
		_Exit(EXIT_FAILURE);
	}

	//create child process	
	if((child = fork()) == 0){
		char ** cmd = (char **)malloc(MAX_COMMANDS_LEN*sizeof(char*));
		cmd = pars_args(command[0], " ", &argc_num);
		//redirect input
		dup2(fd, STDIN_FILENO);
		close(fd);
		//execute command
		if(execvp(cmd[0], cmd) < 0){
            perror("execvp()\n");
			_Exit(EXIT_FAILURE);
		}
	}
	//fork error
	else if(child < 0){
			perror("fork()\n");
			_Exit(EXIT_FAILURE);
	}
	//parent waits until child terminates
    else{
		waitpid(child, &status, WUNTRACED);
		close(fd);
	}
    return 1;
}

//this function determine which function for executing commands is selected
int execute_commands(char ** command, char * cmd, int argc)
{	
	int ret_val;
	//number of commands == 0	
	if(argc < 1)
		return 0;
	//number of commands == 1		
	else if(argc == 1){
		//check for builtin command
		if(check_builtin(command[0])){
			ret_val = execute_builtin(command[0]);	//execute builtin command
			return ret_val;	
		}
		else{
			ret_val = exec_cmd(command[0], argc);	//execute command			
		}
	}
	//number of commands >= 2	
	else if(argc >= 2){
		//a < b
		if(redirect_input(cmd) && !found_pipes(cmd) && !redirect_output(cmd) && argc == 2){
			ret_val = exec_redirect_in(command, argc);
		}
		//a | b > c
		else if(redirect_output(cmd) && !redirect_input(cmd)){
			ret_val = exec_pipe_out(command, argc, true);
		}
		//a | b | c
		else if(found_pipes(cmd) && !redirect_output(cmd) && !redirect_input(cmd)){
			ret_val = exec_pipe_out(command, argc, false);
		}
		//Ambiguous output/input redirect.
		else{
			printf("Ambiguous output/input redirect.\n");
			return 0;	
		}	
	}
	//Invalid number of Arguments
	else{
		error("Invalid number of Arguments");
		return 0;	
	}  	
	return ret_val; 
}

//main shell loop
void shell_loop(int port, char * sock_path, char * IP_addr, int mode)
{
	char * usr_input, * cmd = malloc(127*sizeof(char)), buffer[128];
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	char ** split_cmd = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	int argc, split_argc, i, state = 1, connection = 0, read_bytes, c_sock, sock, client_len;
	struct sockaddr_un unix_addr;				//socket for unix server
	struct sockaddr_in inet_server_addr;		//socket for internet server
	struct sockaddr_in inet_client_addr;		//socket for internet client
	//set up Unix domain socket for listening
	if(mode == 3 || mode == 5){
		bzero((char *)&unix_addr, sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strncpy(unix_addr.sun_path, sock_path, 20);
		//create socket
		if((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
			perror("socket()");
			exit(1);
		}
		unlink(sock_path);
		//binding socket
		if((bind(sock, (struct sockaddr *)&unix_addr, sizeof(unix_addr))) < 0){
			perror("bind()");
			exit(1);
		}
		connection = 1;
		//listen for connection
		listen(sock, 1);
		//accept connection
		if((c_sock = accept(sock, NULL, NULL)) < 0){
			perror("accept()");
			exit(1);
		}
		//if mode 5 unix server deamon redirect stdin to unix socket
		if(mode == 5){
			dup2(c_sock, STDOUT_FILENO);
			write(1, print_prompt(), 50);
		}
			
	}
	//set up internet socket for listening
	if(mode == 4 || mode == 6){
		if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
			perror("socket()");
			exit(1);
		}
		bzero((char *)&inet_server_addr, sizeof(inet_server_addr));
		bzero((char *)&inet_client_addr, sizeof(inet_client_addr));
		inet_server_addr.sin_family = AF_INET;
		inet_server_addr.sin_port = htons(port);
		//check if IP adress is set
		if(strlen(IP_addr) == 0)
            	inet_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");		//default IP address
        else
            	inet_server_addr.sin_addr.s_addr = inet_addr(IP_addr);			//custom IP address

		//binding socket
		if((bind(sock, (struct sockaddr *)&inet_server_addr, sizeof(inet_server_addr))) < 0){
			perror("bind()");
			exit(1);
		}
		connection = 1;
		//listen for connection
		listen(sock, 1);
		client_len = sizeof(inet_client_addr);
		//accept connection
		if((c_sock = accept(sock, (struct sockaddr *)&inet_client_addr, &client_len)) < 0){
			perror("accept()");
			exit(1);
		}
		//if mode 6 internet server deamon redirect stdin to inetrnet socket
		if(mode == 6){
			dup2(c_sock, STDOUT_FILENO);
			write(1, print_prompt(), 50);
		}	
	}

	//main loop for input commands
	while(state){
		//if client is connected on server
		if(connection == 1){
			if(mode == 3 || mode == 4)
				write(c_sock, print_prompt(), 50);					//send prompt to client
			read_bytes = read(c_sock, &buffer, sizeof(buffer));		//read input from client
			buffer[read_bytes] = '\0';
			//closing connection
			if(strcmp(buffer, "quit") == 0){
				write(c_sock, "Closing connection...\n", 22);
				close(sock);
				close(c_sock);
				connection = 0;
				//after closing connection, shell from mode 3 or 4 switch to normal mode 0
				if(mode == 3 || mode == 4)
					continue;
				return;
			}
			usr_input = buffer;
		}
		else{
			usr_input = readline(print_prompt());			//read stdin
		}
		//pressed enter
		if(strlen(usr_input) == 0){
			continue;
		}
		//command length exceeded
		else if(strlen(usr_input) > 128){
			error("MAX_COMMANS_LEN exceeded");
			return;
		}
		//parsing signs \ and #
		else{
			usr_input = cut_comment(usr_input);
			commands = pars_args(usr_input, ";", &argc); 			//parsing commands with ';' separator	
			//in loop every compound commands is parsed with separators |,<,>
			for(i = 0; i < argc; i++){
				strcpy(cmd, commands[i]);
				split_cmd = pars_args(commands[i], "|<>", &split_argc); 	//parsing commands with '|,<,>' separators	
				state = execute_commands(split_cmd, cmd, split_argc);		//executing parsed commands
			}
		}
		free(usr_input);
	}
}

