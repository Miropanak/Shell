#include <stdbool.h>

#ifndef HEADER_FILE
#define HEADER_FILE

void error(char * msg);
void shell_loop();
char ** pars_args(char * line, char * separator)
void help_msg();
void help();
void quit();
void halt();
bool check_builtin(char * command);
bool is_number(char * num);
bool check_port(char * port_num);
bool check_sock_path();
int check_args(int argc, char **argv, int * port, char * sock_path, int * client);
char * get_user();
char * get_host();
char * get_time();
char * print_prompt();

#endif