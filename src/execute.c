#include "../include/execute.h"
#include "builtins.h"
#include "shell.h"
#include <string.h>
#include <unistd.h>

/**
 * exec_command
 * <p>
 * Attempt to execute a command.
 * </p>
 * @param state the state object
 * @param command the command object; contains the command to execute
 * @param path the path upon which to search for the executable
 * @param cmd_len the length of the command field
 * @return 0 if process found and executed.
 */
int exec_command(struct state *state, struct command *command, char *const *path, size_t cmd_len);

/**
 * fork_and_exec
 * <p>
 * Fork the process and replace the child with the command process if found.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @param command the command object
 * @param path the path upon which to find the object
 */
void fork_and_exec(const struct supervisor *supvis, struct state *state, struct command *command, char **path);

/**
 * child_parse_path_and_exec
 * <p>
 * Parse the path to find the executable and execute the executable.
 * </p>
 * @param state the state object
 * @param command the command object
 * @param path the path upon which to find the object
 */
void child_parse_path_exec(struct state *state, struct command *command, char **path);

/**
 * parent_wait
 * <p>
 * Wait for the child process to terminate. Store the return value in the state object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @param command the command object
 * @param pid the pid of the child process
 */
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
        status = exec_command(state, command, path, cmd_len);
    }
}

int exec_command(struct state *state, struct command *command, char *const *path, size_t cmd_len)
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
