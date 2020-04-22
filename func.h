#include <stdbool.h>

#ifndef HEADER_FILE
#define HEADER_FILE

void error(char * msg);
void shell_loop(int port, char * sock_path);
void help_msg();
void help();
void quit();
void unix_client(char * sock_path);
void internet_client(int port);
bool redirect_input(char * line);
bool redirect_output(char * line);
bool found_pipes(char * line);
bool pipe_redir_out(char * line);
bool check_builtin(char * command);
int check_args(int argc, char **argv, int * port, char * sock_path, int * client);
char * get_file_name(char * name);
char * print_prompt();
//void print_args(char ** line, int argc);

#endif
