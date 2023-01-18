#include "../include/execute.h"
#include "builtins.h"
#include "shell.h"
#include <string.h>
#include <unistd.h>

int do_execute_commands(struct supervisor *supvis, struct state *state)
{
    int ret_val;
    
    ret_val = execute(supvis, state, state->command, state->path);
    
    return ret_val;
}

int execute(struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    if (strcmp(command->command, "cd") == 0)
    {
        builtin_cd(supvis, command, state->stderr);
    } else if (strcmp(command->command, "exit") == 0)
    {
        return EXIT;
    } else
    {
        int status;
        
        for (size_t dir_index = 0; *path || status == -1; ++path)
        {
            
            
            status = execv(, command->argv);
        }
    }
    
    
}
