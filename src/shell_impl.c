#include "../include/shell.h"
#include "../include/shell_impl.h"
#include "../include/util.h"

int init_state(struct supervisor *supvis, void *arg)
{
    
}

int destroy_state(struct supervisor *supvis, void *arg)
{
    do_reset_state(supvis, arg);
    
    return EXIT;
}
