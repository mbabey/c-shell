#ifndef CSH_SUPERVISOR_H
#define CSH_SUPERVISOR_H

#include <dc_env/env.h>
#include <dc_error/error.h>
#include <mem_manager/manager.h>

/**
 * struct supervisor
 * <p>
 * Object containing objects passed throughout the program.
 * Contains a dc_env, a dc_error, and a mem_manager.
 * </p>
 */
struct supervisor
{
    struct dc_env *env;
    struct dc_error *err;
    struct memory_manager *mm;
};

struct supervisor *init_supervisor(void);

#endif //CSH_SUPERVISOR_H
