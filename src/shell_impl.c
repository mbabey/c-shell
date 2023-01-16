#include "../include/shell.h"
#include "../include/shell_impl.h"
#include "../include/util.h"
#include "../include/input.h"

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
    int ret_val;
    
    do_read_commands(supvis, arg);
    
    ret_val = (dc_error_has_no_error(supvis->err)) ? SEPARATE_COMMANDS : ERROR;
    
    return ret_val;
}
