#include "../include/builtins.h"

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

int builtin_cd(struct command *command, FILE *ostream)
{
    int exit_code;
    
    if (*(command->argv + 1))
    {
        exit_code = chdir(*(command->argv + 1));
    } else
    {
        char *home;
        
        home = getenv("HOME");
        
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
