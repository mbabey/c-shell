#include "../include/builtins.h"
#include "../include/execute.h"
#include <unistd.h>

int builtin_cd(struct supervisor *supvis, struct command *command, FILE *errstream)
{
    int exit_code;
    
    chdir(*(command->argv + 1));
    fprintf(stderr, "%d ", errno);
    perror("chdir");
    
    exit_code = get_exit_code(errno, NULL, NULL);
    
    return exit_code;
}
