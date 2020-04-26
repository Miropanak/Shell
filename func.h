#include <stdbool.h>

#ifndef HEADER_FILE
#define HEADER_FILE

//error message function
void error(char * msg);
//man shell loop
void shell_loop(int port, char * sock_path, char * IP_addr, int mode);
//help message/ man page
char * help_msg();
//make deamon from shell
void create_deamon();
//main unix client function
void unix_client(char * sock_path);
//main internet client function
void internet_client(int port, char * IP_addr);
//check for sign '<'
bool redirect_input(char * line);
//check for sigh '>'
bool redirect_output(char * line);
//counting pipes in command
int count_pipes(char * command);
//finding pipes in command
bool found_pipes(char * line);
//check for builtin command
bool check_builtin(char * command);
//checking for OPTIONS [-ciudph] and arguments IP_address, port_number and socket_path
int check_args(int argc, char **argv, int * port, char * sock_path, char * IP_addr, int * mode);
//get file name without spaces
char * get_file_name(char * name);
//printing prompt
char * print_prompt();

#endif
