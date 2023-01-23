#ifndef CSH_EXECUTE_H
#define CSH_EXECUTE_H

#include "command.h"
#include "supervisor.h"

/**
 * do_execute_commands
 * <p>
 * Run the execute function.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @return RESET_STATE or DESTROY_STATE or ERROR
 */
int do_execute_commands(struct supervisor *supvis, struct state *state);

/**
 * do_handle_error
 * <p>
 * Handle an error produced by executing a command. Set errno to 0.
 * If state->fatal_error is set, return DESTROY_STATE. Otherwise, return RESET_STATE.
 * </p>
 * @param state the state object
 * @return RESET_STATE or DESTROY_STATE
 */
int do_handle_error(struct state *state);

#endif //CSH_EXECUTE_H
