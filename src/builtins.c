#include "../include/builtins.h"
#include "../include/execute.h"
#include <unistd.h>

void cd_error_message(int err_code, FILE *ostream);

int builtin_cd(struct supervisor *supvis, struct command *command, FILE *ostream)
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
        cd_error_message(errno, ostream);
    }
    
    return exit_code;
}

void cd_error_message(int err_code, FILE *ostream)
{
    switch(err_code)
    {
        case ENOENT:
        {
            fprintf(ostream, "cd: no such file or directory\n");
            break;
        }
        case EACCES:
        {
            fprintf(ostream, "cd: permission denied\n");
            break;
        }
        case ENAMETOOLONG:
        {
            fprintf(ostream, "cd: file name too long\n");
            break;
        }
        case ELOOP:
        {
            fprintf(ostream, "cd: too many levels of symbolic links\n");
            break;
        }
        case ENOTDIR:
        {
            fprintf(ostream, "cd: not a directory\n");
            break;
        }
        case EBUSY:
        {
            fprintf(ostream, "cd: device / resource busy\n");
            break;
        }
        case EINVAL:
        {
            fprintf(ostream, "cd: HOME environment variable is undefined\n");
            break;
        }
        case ENOMEM:
        {
            fprintf(ostream, "cd: unable to allocate memory for the environment\n");
            break;
        }
        default:
        {
            fprintf(ostream, "cd: undefined error\n");
            break;
        }
    }
}
