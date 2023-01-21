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
    int ret_val;
    
    if (strcmp(command->command, "cd") == 0)
    {
        ret_val = builtin_cd(supvis, command, state->stderr);
    } else if (strcmp(command->command, "exit") == 0)
    {
        ret_val = EXIT;
    } else
    {
        size_t cmd_len;
        size_t len;
        int    status;
        char   *path_and_cmd;
        
        cmd_len = strlen(command->command);
        
        for (; *path || status == 0; ++path)
        {
            len          = strlen(*path) + cmd_len + 1;
            path_and_cmd = (char *) malloc(len);
            strcpy(path_and_cmd, *path);
            strcat(path_and_cmd, command->command);
            
            status = execv(path_and_cmd, command->argv);
            if (status == -1 && errno == ENOENT)
            {
                errno = 0;
            } else if (status == -1)
            {
                state->fatal_error = true;
            }
            
            free(path_and_cmd);
        }
        
        ret_val = (state->fatal_error) ? ERROR : RESET_STATE;
    }
    
    return ret_val;
}
