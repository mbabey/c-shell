#include "../include/builtins.h"

#include <string.h>
#include <unistd.h>

/**
 * cd_error_message
 * <p>
 * Print an error message based on the value of errno passed to this function.
 * </p>
 * @param err_code the errno passed
 * @param arg the argument to chdir in builtin_cd
 * @param ostream the stream on which to print the message
 */
void cd_error_message(int err_code, const char *arg, FILE *ostream);

/**
 * find_command
 * <p>
 * Assemble a path directory and a command together, then check if it exists.
 * </p>
 * @param cmd the command to check for existence
 * @param path the path on which to check existence
 * @param cmd_len the length of the command
 * @return 0 if found, -1 otherwise
 */
char *find_command(const char *cmd, const char *path, size_t cmd_len);

/**
 * cd_error_message
 * <p>
 * Print an error message based on the value of errno passed to this function.
 * </p>
 * @param err_code the errno passed
 * @param cmd the command searched for by which
 * @param ostream the stream on which to print the message
 */
void which_err_message(int err_code, const char *cmd, FILE *ostream);

int builtin_cd(struct command *command, FILE *ostream)
{
    int exit_code;
    
    if (*(command->argv + 1))
    {
        exit_code = chdir(*(command->argv + 1));
    } else
    {
        char *home;
        
        home = getenv("HOME"); // NOLINT(concurrency-mt-unsafe): no threads here
        
        exit_code = (home) ? chdir(home) : -1;
    }
    
    if (exit_code == -1)
    {
        cd_error_message(errno, *(command->argv + 1), ostream);
    }
    
    return exit_code;
}

void cd_error_message(int err_code, const char *arg, FILE *ostream)
{
    switch(err_code)
    {
        case ENOENT:
        {
            (void) fprintf(ostream, "cd: no such file or directory: %s\n", arg);
            break;
        }
        case EACCES:
        {
            (void) fprintf(ostream, "cd: permission denied: %s\n", arg);
            break;
        }
        case ENAMETOOLONG:
        {
            (void) fprintf(ostream, "cd: file name too long: %s\n", arg);
            break;
        }
        case ELOOP:
        {
            (void) fprintf(ostream, "cd: too many levels of symbolic links: %s\n", arg);
            break;
        }
        case ENOTDIR:
        {
            (void) fprintf(ostream, "cd: not a directory: %s\n", arg);
            break;
        }
        case EBUSY:
        {
            (void) fprintf(ostream, "cd: device / resource busy: %s\n", arg);
            break;
        }
        case EINVAL:
        {
            (void) fprintf(ostream, "cd: HOME environment variable is undefined\n");
            break;
        }
        case ENOMEM:
        {
            (void) fprintf(ostream, "cd: unable to allocate memory for the environment\n");
            break;
        }
        default:
        {
            (void) fprintf(ostream, "cd: undefined error: %s\n", arg);
            break;
        }
    }
}

int builtin_which(char *cmd, char **path, FILE *ostream)
{
    int status;
    size_t cmd_len;
    char *location;
    
    cmd_len = strlen(cmd);
    
    for (; *path && !location; ++path)
    {
        location = find_command(cmd, *path, cmd_len);
    }
    
    if (location)
    {
        (void) fprintf(ostream, "%s\n", location);
        status = 0;
    } else
    {
        which_err_message(errno, NULL, NULL);
        
        status = -1;
    }
    
    return status;
}

char *find_command(const char *cmd, const char *path, const size_t cmd_len)
{
    size_t len;
    char   *path_and_cmd;
    int    status;
    
    len          = strlen(path) + cmd_len + 2;
    path_and_cmd = (char *) malloc(len);
    strcpy(path_and_cmd, path);
    strcat(path_and_cmd, "/");
    strcat(path_and_cmd, cmd);
    
    status = access(path_and_cmd, F_OK);
    
    if (!status)
    {
        return path_and_cmd;
    }
    
    free(path_and_cmd);
    
    return NULL;
}

void which_err_message(int err_code, const char *cmd, FILE *ostream)
{
    switch (err_code)
    {
        case EACCES:
        {
            (void) fprintf(ostream, "which: permission denied: %s\n", cmd);
            break;
        }
        case ELOOP:
        {
            (void) fprintf(ostream, "which: too many levels of symbolic links: %s\n", cmd);
            break;
        }
        case ENAMETOOLONG:
        {
            (void) fprintf(ostream, "which: file name too long: %s\n", cmd);
            break;
        }
        case ENOENT:
        {
            (void) fprintf(ostream, "%s not found\n", cmd);
            break;
        }
        case ENOTDIR:
        {
            (void) fprintf(ostream, "which: not a directory: %s\n", cmd);
            break;
        }
        case EROFS:
        {
            (void) fprintf(ostream, "which: read-only file system: %s\n", cmd);
            break;
        }
        default:
        {
            (void) fprintf(ostream, "which: undefined error: %s\n", cmd);
        }
    }
}
