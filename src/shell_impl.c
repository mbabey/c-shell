#include "../include/command.h"
#include "../include/execute.h"
#include "../include/input.h"
#include "../include/shell.h"
#include "../include/shell_impl.h"
#include "../include/util.h"

int init_state(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_init_state(supvis, arg);
    
    ret_val = (errno) ? ERROR : READ_COMMANDS;
    
    return ret_val;
}

int read_commands(struct supervisor *supvis, void *arg)
{
    int    ret_val;
    size_t line_size;
    
    line_size = do_read_commands(supvis, arg);
    
    if (errno)
    {
        ret_val = ERROR;
    } else if (line_size == 1)
    {
        ret_val = RESET_STATE;
    } else
    {
        ret_val = SEPARATE_COMMANDS;
    }
    
    return ret_val;
}

int separate_commands(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_separate_commands(supvis, arg);
    
    ret_val = (errno) ? ERROR : PARSE_COMMANDS;
    
    return ret_val;
}

int parse_commands(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_parse_commands(supvis, arg);
    
    ret_val = (errno) ? ERROR : EXECUTE_COMMANDS;
    
    return ret_val;
}

int execute_commands(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    ret_val = do_execute_commands(supvis, arg);
    
    return ret_val;
}

int reset_state(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_reset_state(supvis, arg);
    
    ret_val = (errno) ? ERROR : READ_COMMANDS;
    
    return ret_val;
}

int handle_error(void *arg)
{
    int ret_val;
    
    ret_val = do_handle_error(arg);
    
    return ret_val;
}

void destroy_state(struct supervisor *supvis, void *arg)
{
    do_destroy_state(supvis, arg);
}
