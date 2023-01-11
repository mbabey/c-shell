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

/**
 * init_supervisor
 * <p>
 * Allocate and initialize a supervisor with a dc_env, a dc_error, and
 * a memory_manager. Add the dc_env and dc_error to the memory_manager.
 * If allocation of the supervisor fails, return NULL.
 * </p>
 * @return a pointer to the new supervisor, NULL if failure
 */
struct supervisor *init_supervisor(void);

/**
 * destroy_supervisor
 * <p>
 * Destroy a supervisor by freeing all memory contained in its memory_manager
 * and the supervisor itself.
 * </p>
 * @param supervisor the supervisor to destroy
 */
void destroy_supervisor(struct supervisor *supervisor);

#endif //CSH_SUPERVISOR_H
