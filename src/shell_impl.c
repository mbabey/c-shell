#include "../include/shell.h"
#include "../include/shell_impl.h"
#include "../include/util.h"
#include "../include/input.h"
#include "../include/command.h"
#include "../include/execute.h"

int init_state(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    arg = do_init_state(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? READ_COMMANDS : ERROR;
    
    return ret_val;
}

int destroy_state(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_destroy_state(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? EXIT : ERROR;
    
    return ret_val;
}

int reset_state(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_reset_state(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? READ_COMMANDS : ERROR;
    
    return ret_val;
}

int read_commands(struct supervisor *supvis, void *arg)
{
    int    ret_val;
    size_t line_size;
    
    line_size = do_read_commands(supvis, arg);
    
    if (dc_error_has_error(supvis->err))
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
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? PARSE_COMMANDS : ERROR;
    
    return ret_val;
}

int parse_commands(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_parse_commands(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? EXECUTE_COMMANDS : ERROR;
    
    return ret_val;
}

int execute_commands(struct supervisor *supvis, void *arg)
{
    int ret_val;
    
    do_execute_commands(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? RESET_STATE : ERROR;
    
    return ret_val;
}
