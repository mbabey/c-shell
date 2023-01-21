#include "../include/execute.h"
#include "builtins.h"
#include "shell.h"
#include <string.h>
#include <unistd.h>

#define EXIT_EACCES 4
#define EXIT_EFAULT 5
#define EXIT_EINVAL 6
#define EXIT_ENOTDIR 7
#define EXIT_ELOOP 8
#define EXIT_ENAMETOOLONG 9
#define EXIT_ENOEXEC 10
#define EXIT_E2BIG 69
#define EXIT_ENOENT 127

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
void fork_and_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path);

/**
 * child_parse_path_and_exec
 * <p>
 * Parse the path to find the executable and execute the executable.
 * </p>
 * @param state the state object
 * @param command the command object
 * @param path the path upon which to find the object
 */
void child_parse_path_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path);

/**
 * get_exit_code
 * <p>
 * Get an exit code for the child process based on the result of running execv.
 * Print an error message to state->stderr based on the set errno.
 * </p>
 * @return the exit code
 */
int get_exit_code(const char *command, FILE *err);

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

void fork_and_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    pid_t pid;
    
    pid = fork();
    
    if (pid < 0)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    } else if (pid == 0)
    {
        child_parse_path_exec(supvis, state, command, path);
    } else
    {
        parent_wait(supvis, state, command, pid);
    }
}

void child_parse_path_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    size_t cmd_len;
    int    status;
    int    exit_code;
    
    cmd_len = strlen(command->command);
    
    status = 1;
    for (; *path || status == 0; ++path)
    {
        status = exec_command(state, command, path, cmd_len);
    }
    
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
    
    exit_code = get_exit_code(command->command, state->stderr);
    
    exit(exit_code);
}

int get_exit_code(const char *command, FILE *err)
{
    int exit_code;
    
    switch (errno)
    {
        case ENOENT:
        {
            exit_code = EXIT_ENOENT;
            fprintf(err, "csh: command not found: %s", command);
            break;
        }
        case EACCES:
        {
            exit_code = EXIT_EACCES;
            fprintf(err, "csh: permission denied: %s", command);
            break;
        }
        case EFAULT:
        {
            exit_code = EXIT_EFAULT;
            fprintf(err, "csh: arguments outside address space: %s", command);
            break;
        }
        case EINVAL:
        {
            exit_code = EXIT_EINVAL;
            fprintf(err, "csh: execution not supported by this system: %s", command);
            break;
        }
        case ENOTDIR:
        {
            exit_code = EXIT_ENOTDIR;
            fprintf(err, "csh: no such file or directory: %s", command);
            break;
        }
        case ELOOP:
        {
            exit_code = EXIT_ELOOP;
            fprintf(err, "csh: symbolic loop detected: %s", command);
            break;
        }
        case ENAMETOOLONG:
        {
            exit_code = EXIT_ENAMETOOLONG;
            fprintf(err, "csh: argument name too long: %s", command);
            break;
        }
        case ENOEXEC:
        {
            exit_code = EXIT_ENOEXEC;
            fprintf(err, "csh: unrecognized format: %s", command);
            break;
        }
        case E2BIG:
        {
            exit_code = EXIT_E2BIG;
            fprintf(err, "csh: too many arguments and/or environment variables: %s", command);
            break;
        }
        default:
        {
            exit_code = EXIT_FAILURE;
            fprintf(err, "csh: undefined error: %s", command);
            break;
        }
    }
    
    return exit_code;
}

int exec_command(struct state *state, struct command *command, char *const *path, size_t cmd_len)
{
    size_t len;
    char   *path_and_cmd;
    int    status;
    
    len          = strlen(*path) + cmd_len + 2;
    path_and_cmd = (char *) malloc(len);
    strcpy(path_and_cmd, *path);
    strcat(path_and_cmd, "/");
    strcat(path_and_cmd, command->command);
    
    printf("%s\n", path_and_cmd);
    
    status = execv(path_and_cmd, command->argv);
    if (status == -1 && errno != ENOENT)
    {
        state->fatal_error = true;
    }
    
    free(path_and_cmd);
    return status;
}

void parent_wait(const struct supervisor *supvis, struct state *state, struct command *command, pid_t pid)
{
    pid_t wait_ret;
    int   ret_val;
    
    wait_ret = waitpid(pid, &ret_val, 0);
    if (wait_ret == -1)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    } else if (WIFEXITED(ret_val))
    {
        command->exit_code = WEXITSTATUS(ret_val);
        fprintf(state->stdout, "%s exited with status %d\n", command->command, command->exit_code);
    }
}
