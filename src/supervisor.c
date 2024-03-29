#include "../include/supervisor.h"

#include <mem_manager/manager.h>

#include <stdlib.h>

struct supervisor *init_supervisor(void)
{
    struct supervisor     *supvis;
    struct dc_env         *env;
    struct dc_error       *err;
    struct memory_manager *mm;
    
    supvis = (struct supervisor *) malloc(sizeof(struct supervisor));
    
    if (supvis)
    {
        err = dc_error_create(true);
        env = dc_env_create(err, false, NULL);
        mm  = init_mem_manager();
        
        mm->mm_add(mm, err);
        mm->mm_add(mm, env);
        
        supvis->env = env;
        supvis->err = err;
        supvis->mm  = mm;
    }
    
    return supvis;
}

void destroy_supervisor(struct supervisor *supervisor)
{
    free_mem_manager(supervisor->mm);
    free(supervisor);
}
