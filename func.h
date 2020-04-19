#ifndef HEADER_FILE
#define HEADER_FILE

void error(char * msg);
void shell_loop();
bool is_number(char * num);
bool check_port(char * port_num);
bool check_sock_path();
int check_args(int argc, char **argv, int * port, char * sock_path);
void help_msg();
char * get_user();
char * get_host();
char * get_time();
char * print_prompt();

#endif