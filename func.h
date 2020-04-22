#include <stdbool.h>

#ifndef HEADER_FILE
#define HEADER_FILE

void error(char * msg);
void shell_loop(int port, char * sock_path, char * IP_addr, int mode);
void help_msg();
void help();
void unix_client(char * sock_path);
void internet_client(int port, char * IP_addr);
bool redirect_input(char * line);
bool redirect_output(char * line);
bool found_pipes(char * line);
bool pipe_redir_out(char * line);
bool check_builtin(char * command);
int check_args(int argc, char **argv, int * port, char * sock_path, char * IP_addr, int * mode);
char * get_file_name(char * name);
char * print_prompt();
//void print_args(char ** line, int argc);

#endif
