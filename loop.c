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
	return new_line;	
}

int execute_builtin(char * command)
{
	if(strcmp(command, "help") == 0)
		help_msg();
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

int exec_cmd(char * command, int argc)
{	
	int num_of_args, status;
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	pid_t child_pid;
	commands = pars_args(command, " ", &num_of_args);
	child_pid = fork();
	if(child_pid < 0){
		perror("fork()");
	}
	else if(child_pid == 0){
		if(execvp(commands[0], commands) < 0){
			perror("execvp()");
			_Exit(EXIT_FAILURE);
		}		
	}
	else{
		waitpid(child_pid, &status, WUNTRACED);
	}
	return 1;
}

int exec_multi_pipes(char ** command, int argc)
{
	int i, num_of_pipes, *pipe_fd, status, argc_num, counter = 0;
	pid_t child;
	num_of_pipes = argc - 1;
    pipe_fd = malloc(num_of_pipes*2*sizeof(int));
	//create pipes
	for(i = 0; i < num_of_pipes; i++){
        if((pipe(pipe_fd + i*2)) < 0){
            perror("pipe()");
            exit(1);
        }   
    }
	printf("num od pipes: %d\n", num_of_pipes);
    while(counter < argc){
        //child
        if((child = fork()) == 0){
			char ** cmd = (char **)malloc(MAX_COMMANDS_LEN*sizeof(char*));
			cmd = pars_args(command[counter], " ", &argc_num);
			print_args(cmd, argc_num);
            //first command
            if(counter == 0){
				printf("First cmd:\n");
                dup2(pipe_fd[1], STDOUT_FILENO);
            }
            //last command
            else if(counter == argc-1){
				printf("Last cmd:\n");
                dup2(pipe_fd[counter*2-2], STDIN_FILENO);
            }
            //middle commands
            else{
				printf("Middle cmd:\n");
                dup2(pipe_fd[counter*2-2], STDIN_FILENO);
                dup2(pipe_fd[counter*2+1], STDOUT_FILENO);
            }
			for(i = 0; i < num_of_pipes*2; i++)
				if(close(pipe_fd[i]) < 0)
					perror("Close()");
            if(execvp(cmd[0], cmd) < 0)
                perror("execvp()");
            exit(1);
        }
        //fork error
        else if(child < 0){
            perror("fork()");
            exit(1);
        } 
        //parent  
        else{
            printf("counter: %d\n", counter);
            counter++;
        }
    }
	for(i = 0; i < num_of_pipes*2; i++)
        close(pipe_fd[i]);
    for (i = 0; i < 3; i++)
        wait(&status);

	return 1;
}

int exec_redirect_out(char ** command, int argc, bool out)
{
	int i, num_of_pipes, *pipe_fd, status, argc_num = argc, counter = 0, fd;
	pid_t child;
	if(out){
		num_of_pipes = argc - 2;
		char *file_name = malloc(20*sizeof(char));
		if(strlen(command[argc - 1]) > 19){
			error("Too long name of file");
			exit(1);
		}
		if((file_name = get_file_name(command[argc_num -1])) == NULL)
			exit(1);
		if((fd = open(file_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) < 0){
			perror("open()");
			exit(1);
		}
	}
		
	else
		num_of_pipes = argc - 1;
	
    pipe_fd = malloc(num_of_pipes*2*sizeof(int));
	//create pipes
	for(i = 0; i < num_of_pipes; i++){
        if((pipe(pipe_fd + i*2)) < 0){
            perror("pipe()");
            exit(1);
        }   
    }
	printf("num od pipes: %d\n", num_of_pipes);
    while(counter < argc){
        //child
        if((child = fork()) == 0){
			char ** cmd = (char **)malloc(MAX_COMMANDS_LEN*sizeof(char*));
			cmd = pars_args(command[counter], " ", &argc_num);
			print_args(cmd, argc_num);
            //first command
            if(counter == 0){
				printf("First cmd:\n");
				if(out && argc)
					dup2(fd, STDOUT_FILENO);
                else
					dup2(pipe_fd[1], STDOUT_FILENO);
            }
            //last command
            else if(counter == argc-1){
				printf("Last cmd:\n");
              	dup2(pipe_fd[counter*2-2], STDIN_FILENO);
            }
            //middle commands
            else{
				printf("Middle cmd:\n");
				dup2(pipe_fd[counter*2-2], STDIN_FILENO);
				if(out && counter == argc-2)
					dup2(fd, STDOUT_FILENO);
				else
                	dup2(pipe_fd[counter*2+1], STDOUT_FILENO);
            }
			if(out && counter == argc-2)
				close(fd);
			for(i = 0; i < num_of_pipes*2; i++)
				if(close(pipe_fd[i]) < 0)
					perror("Close()");
            if(execvp(cmd[0], cmd) < 0)
                perror("execvp()");
            exit(1);
        }
        //fork error
        else if(child < 0){
            perror("fork()");
            exit(1);
        } 
        //parent  
        else{
			if(out && counter == argc-2)
				break;
            printf("counter: %d\n", counter);
            counter++;
        }
    }
	if(out)
		close(fd);
	for(i = 0; i < num_of_pipes*2; i++)
        close(pipe_fd[i]);
    for (i = 0; i < 3; i++)
        wait(&status);

	return 1;
}

int exec_redirect_inout(char ** command, int argc, int in_out)
{
	int fd, status, argc_num;
	pid_t child;
	char *file_name = malloc(20*sizeof(char));
	if(strlen(command[argc-1]) > 19){
		error("Too long name of file");
		exit(1);
	}
	if((file_name = get_file_name(command[argc-1])) == NULL)
		exit(1);

	if(in_out == 0){
		if((fd = open(file_name, O_RDONLY, S_IRUSR | S_IWUSR)) < 0){
			perror("open()");
			_Exit(EXIT_FAILURE);
		}	
	}
	else if (in_out == 1){
		if((fd = open(file_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) < 0){
			perror("open()");
			_Exit(EXIT_FAILURE);
		}
	}
	else
		return 0;
		
	if((child = fork()) == 0){
		char ** cmd = (char **)malloc(MAX_COMMANDS_LEN*sizeof(char*));
		cmd = pars_args(command[0], " ", &argc_num);
		close(in_out);
		dup(fd);
		if(execvp(cmd[0], cmd) < 0){
            perror("execvp()\n");
			_Exit(EXIT_FAILURE);
		}
        else if(child < 0){
                perror("fork()\n");
                _Exit(EXIT_FAILURE);
        }
        else
            waitpid(child, &status, WUNTRACED);
	}
	close(fd);

    return 1;
}

int execute_commands(char ** command, char * cmd, int argc)
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
	else if(argc >= 2){
		if(redirect_output(cmd) && !found_pipes(cmd) && !redirect_input(cmd)){
        		//printf("a > b\n");
				//ret_val = exec_redirect_inout(command, argc, 1);
				ret_val = exec_redirect_out(command, argc, true);
    		}
    		//<
    		else if(redirect_input(cmd) && !found_pipes(cmd) && !redirect_output(cmd)){
        		//printf("a < b\n");
    			ret_val = exec_redirect_inout(command, argc, 0);
		}
    		//||>
    		else if(redirect_output(cmd) && found_pipes(cmd) && !redirect_input(cmd) && pipe_redir_out(cmd)){
        		//printf("a | b > c\n");
				ret_val = exec_redirect_out(command, argc, true);
    		}
    		//|||
    		else if(found_pipes(cmd) && !redirect_output(cmd) && !redirect_input(cmd)){
        		//printf("a | b | c\n");
				ret_val = exec_multi_pipes(command, argc);
    		}
    		else{
        		printf("Ambiguous output/input redirect.\n");
    			return 0;	
		}	
	}
	else{
		error("Invalid number of Arguments");
		return 0;	
	}  	
	return ret_val; 
}

void shell_loop(int port, char * sock_path, char * IP_addr, int mode)
{
	char * usr_input, * cmd = malloc(127*sizeof(char)), buffer[128];
	char ** commands = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	char ** split_cmd = (char **)malloc(MAX_NUM_OF_COMMANDS*sizeof(char *));
	int argc, split_argc, i, state = 1, connection = 0, read_bytes, c_sock, sock, client_len;
	struct sockaddr_un unix_addr;
	struct sockaddr_in inet_server_addr;
	struct sockaddr_in inet_client_addr;
	//set up Unix domain socket for listening
	if(mode == 3 || mode == 5){
		bzero((char *)&unix_addr, sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strncpy(unix_addr.sun_path, sock_path, 20);
		if((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
			perror("socket()");
			exit(1);
		}
		unlink(sock_path);
		if((bind(sock, (struct sockaddr *)&unix_addr, sizeof(unix_addr))) < 0){
			perror("bind()");
			exit(1);
		}
		connection = 1;
		listen(sock, 5);
		if((c_sock = accept(sock, NULL, NULL)) < 0){
			perror("accept()");
			exit(1);
		}
		if(mode == 5){
			dup2(c_sock, STDOUT_FILENO);
			write(c_sock, print_prompt(), 50);
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
		if(strlen(IP_addr) == 0)
            	inet_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        else
            	inet_server_addr.sin_addr.s_addr = inet_addr(IP_addr);

		if((bind(sock, (struct sockaddr *)&inet_server_addr, sizeof(inet_server_addr))) < 0){
			perror("bind()");
			exit(1);
		}
		connection = 1;
		listen(sock, 5);
		client_len = sizeof(inet_client_addr);
		if((c_sock = accept(sock, (struct sockaddr *)&inet_client_addr, &client_len)) < 0){
			perror("accept()");
			exit(1);
		}
		if(mode == 6){
			dup2(c_sock, STDOUT_FILENO);
			write(c_sock, print_prompt(), 50);
		}	
	}

	while(state){
		if(connection == 1){
			if(mode == 3 || mode == 4)
				write(c_sock, print_prompt(), 50);
			read_bytes = read(c_sock, &buffer, sizeof(buffer));
			buffer[read_bytes] = '\0';
			if(strcmp(buffer, "quit") == 0){
				write(c_sock, "Closing connection...\n", 22);
				close(sock);
				close(c_sock);
				connection = 0;
				if(mode == 3 || mode == 4)
					continue;
				return;
			}
			usr_input = buffer;
		}
		else{
			usr_input = readline(print_prompt());
		}
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
			usr_input = cut_comment(usr_input);
			commands = pars_args(usr_input, ";", &argc); 			//parsovanie prikazov podla ';'	
			//kazdy zlozeny prikaz v cykle sa rozdeli podla |,<,>
			for(i = 0; i < argc; i++){
				strcpy(cmd, commands[i]);
				split_cmd = pars_args(commands[i], "|<>", &split_argc);
				state = execute_commands(split_cmd, cmd, split_argc);
			}
		}
		free(usr_input);
	}
}

