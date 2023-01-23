#include "../include/builtins.h"
#include "../include/execute.h"
#include "../include/shell.h"

#include <signal.h>
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
#define EXIT_UNDEFINED 113
#define EXIT_ENOENT 127

/**
 * pid_global
 * <p>
 * Global store for pid to allow signal to be sent to child process.
 * </p>
 */
pid_t pid_global;

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
int exec_command(struct command *command, char *const *path, size_t cmd_len);

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
 * parent_wait
 * <p>
 * Wait for the child process to terminate. Store the return value in the state object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @param command the command object
 * @param pid the pid_global of the child process
 */
void parent_wait(struct state *state, struct command *command);

/**
 * kill_child_handler
 * <p>
 * Signal handler. Sends a kill signal to a the child process.
 * </p>
 * @param signal
 */
void kill_child_handler(int signal);

/**
 * print_err_message
 * <p>
 * Print an error message based on the exit code of the state. The message will be printed
 * to an output defined by out.
 * </p>
 * @param exit_code the exit code of the state
 * @param command the command that caused the error
 * @param ostream the output stream
 */
void print_err_message(int exit_code, const char *command, FILE *ostream);

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
        state->command->exit_code = builtin_cd(command, state->stdout);
        ret_val = (state->command->exit_code) ? ERROR : RESET_STATE;
    } else if (strcmp(command->command, "exit") == 0)
    {
        ret_val = DESTROY_STATE;
    } else
    {
        fork_and_exec(supvis, state, command, path);
        ret_val = (state->command->exit_code) ? ERROR : RESET_STATE;
    }
    
    return ret_val;
}

void fork_and_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    pid_global = fork();
    
    if (pid_global < 0)
    {
        fprintf(state->stderr, "csh: fatal error: could not fork process\n");
        state->fatal_error = true;
        command->exit_code = EXIT_FAILURE;
    } else if (pid_global == 0)
    {
        child_parse_path_exec(supvis, state, command, path);
    } else
    {
        parent_wait(NULL, command);
    }
}

void child_parse_path_exec(struct supervisor *supvis, struct state *state, struct command *command, char **path)
{
    size_t cmd_len;
    int    status;
    int    exit_code;
    
    cmd_len = strlen(command->command);
    
    status = 1;
    for (; *path && status != 0; ++path)
    {
        status = exec_command(command, path, cmd_len);
    }
    
    exit_code = get_exit_code(errno);
    print_err_message(exit_code, state->command->command, state->stdout);
    
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
    
    exit(exit_code);
}

int exec_command(struct command *command, char *const *path, size_t cmd_len)
{
    size_t len;
    char   *path_and_cmd;
    int    status;
    
    len          = strlen(*path) + cmd_len + 2;
    path_and_cmd = (char *) malloc(len);
    strcpy(path_and_cmd, *path);
    strcat(path_and_cmd, "/");
    strcat(path_and_cmd, command->command);
    
    status = execv(path_and_cmd, command->argv);
    
    free(path_and_cmd);
    return status;
}

void parent_wait(struct state *state, struct command *command)
{
    pid_t wait_ret;
    int   ret_val;
    
    signal(SIGINT, kill_child_handler);
    
    wait_ret = waitpid(pid_global, &ret_val, 0);
    if (wait_ret == -1)
    {
        fprintf(state->stderr, "csh: fatal error: could not wait for child process to finish execution.\n");
        state->fatal_error = true;
        command->exit_code = EXIT_FAILURE;
    } else if (WIFEXITED(ret_val))
    {
        command->exit_code = WEXITSTATUS(ret_val);
    }
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void kill_child_handler(int signal)
{
    kill(pid_global, SIGKILL);
}
#pragma GCC diagnostic pop

int do_handle_error(struct state *state)
{
    int ret_val;
    
    errno = 0;
    
    if (state->fatal_error)
    {
        ret_val = DESTROY_STATE;
    } else
    {
        ret_val = RESET_STATE;
    }
    
    return ret_val;
}

int get_exit_code(int err_code)
{
    int exit_code;
    
    switch (err_code)
    {
        case ENOENT:
        {
            exit_code = EXIT_ENOENT;
            break;
        }
        case EACCES:
        {
            exit_code = EXIT_EACCES;
            break;
        }
        case EFAULT:
        {
            exit_code = EXIT_EFAULT;
            break;
        }
        case EINVAL:
        {
            exit_code = EXIT_EINVAL;
            break;
        }
        case ENOTDIR:
        {
            exit_code = EXIT_ENOTDIR;
            break;
        }
        case ELOOP:
        {
            exit_code = EXIT_ELOOP;
            break;
        }
        case ENAMETOOLONG:
        {
            exit_code = EXIT_ENAMETOOLONG;
            break;
        }
        case ENOEXEC:
        {
            exit_code = EXIT_ENOEXEC;
            break;
        }
        case E2BIG:
        {
            exit_code = EXIT_E2BIG;
            break;
        }
        default:
        {
            exit_code = EXIT_UNDEFINED;
            break;
        }
    }
    
    return exit_code;
}

void print_err_message(int exit_code, const char *command, FILE *ostream)
{
    switch (exit_code)
    {
        case EXIT_ENOENT:
        {
            fprintf(ostream, "csh: command not found: %s\n", command);
            break;
        }
        case EXIT_EACCES:
        {
            fprintf(ostream, "csh: permission denied: %s\n", command);
            break;
        }
        case EXIT_EFAULT:
        {
            fprintf(ostream, "csh: bad address: %s\n", command);
            break;
        }
        case EXIT_EINVAL:
        {
            fprintf(ostream, "csh: invalid argument: %s\n", command);
            break;
        }
        case EXIT_ENOTDIR:
        {
            fprintf(ostream, "csh: not a directory: %s\n", command);
            break;
        }
        case EXIT_ELOOP:
        {
            fprintf(ostream, "csh: too many levels of symbolic links: %s\n", command);
            break;
        }
        case EXIT_ENAMETOOLONG:
        {
            fprintf(ostream, "csh: file name too long: %s\n", command);
            break;
        }
        case EXIT_ENOEXEC:
        {
            fprintf(ostream, "csh: exec format error: %s\n", command);
            break;
        }
        case EXIT_E2BIG:
        {
            fprintf(ostream, "csh: argument list too long: %s\n", command);
            break;
        }
        default:
        {
            fprintf(ostream, "csh: undefined error: %s\n", command);
            break;
        }
    }
}

