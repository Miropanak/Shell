#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "func.h"

void shell_loop()
{
        char * usr_input = malloc(128 * sizeof(char));
        usr_input = readline(print_prompt());
}