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
 */
int do_execute_commands(struct supervisor *supvis, struct state *state);

/**
 * execute
 * <p>
 * Create a child process, exec the command with any redirection, and set the exit code.
 * If there is an error executing the command, print an exit code.
 * If the command cannot be found set command->exit_code to 127.
 * </p>
 * @param supvis the supervisor object
 * @param state the state struct
 * @param command the command struct
 * @param path the path
 */
int execute(struct supervisor *supvis, struct state *state, struct command *command, char **path);

/**
 * do_handle_error
 * <p>
 * Handle an error produced by executing a command; print a relevant error message and clean the
 * error struct. If state->fatal_error is set, return DESTROY_STATE. Otherwise, return RESET_STATE.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @return RESET_STATE or DESTROY_STATE
 */
int do_handle_error(struct supervisor *supvis, struct state *state);

#endif //CSH_EXECUTE_H
