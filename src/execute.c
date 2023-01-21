#include "../include/execute.h"
#include "builtins.h"
#include "shell.h"
#include <string.h>
#include <unistd.h>

int do_execute(struct state *state, struct command *command, char *const *path, size_t cmd_len);

void fork_and_exec(const struct supervisor *supvis, struct state *state, struct command *command, char **path);

void child_parse_path_exec(struct state *state, struct command *command, char **path);

void parent_wait(const struct supervisor *supvis, struct state *state, struct command *command, pid_t pid);

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
        builtin_cd(supvis, command, state->stderr);
        ret_val = (state->fatal_error) ? ERROR : RESET_STATE;
    } else if (strcmp(command->command, "exit") == 0)
    {
        ret_val = EXIT;
    } else
    {
        fork_and_exec(supvis, state, command, path);
    
        ret_val = (state->fatal_error) ? ERROR : RESET_STATE;
    }
    
    return ret_val;
}

void fork_and_exec(const struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    pid_t pid;
    
    pid = fork();
    
    if (pid < 0)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    } else if (pid == 0)
    {
        child_parse_path_exec(state, command, path);
    } else
    {
        parent_wait(supvis, state, command, pid);
    }
}

void child_parse_path_exec(struct state *state, struct command *command, char **path)
{
    size_t cmd_len;
    int    status;
    
    cmd_len = strlen(command->command);
    
    status = 1;
    for (; *path || status == 0; ++path)
    {
        status = do_execute(state, command, path, cmd_len);
    }
}

int do_execute(struct state *state, struct command *command, char *const *path, size_t cmd_len)
{
    size_t len;
    char   *path_and_cmd;
    int status;
    
    len          = strlen(*path) + cmd_len + 2;
    path_and_cmd = (char *) malloc(len);
    strcpy(path_and_cmd, *path);
    strcat(path_and_cmd, "/");
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
    return status;
}

void parent_wait(const struct supervisor *supvis, struct state *state, struct command *command, pid_t pid)
{
    pid_t wait_ret;
    
    wait_ret = waitpid(pid, &command->exit_code, 0);
    if (wait_ret == -1)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    } else if (WIFEXITED(command->exit_code))
    {
        fprintf(state->stdout, "%s exited with status %d\n", command->command, WEXITSTATUS(command->exit_code));
    }
}
