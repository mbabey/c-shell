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
 * Return DESTROY_STATE if the command is "exit"; return RESET_STATE if the exit code is 0;
 * return ERROR otherwise.
 * </p>
 * @param supvis the supervisor object
 * @param state the state struct
 * @param command the command struct
 * @param path the path
 * @return DESTROY_STATE or RESET_STATE or ERROR
 */
int execute(struct supervisor *supvis, struct state *state, struct command *command, char **path);

/**
 * get_exit_code
 * <p>
 * Get an exit code for the child process based on the result of running execv.
 * </p>
 * @param err_code the errno
 * @return the exit code
 */
int get_exit_code(int err_code, const char *command, FILE *ostream);

/**
 * do_handle_error
 * <p>
 * Handle an error produced by executing a command; print a relevant error message and clean the
 * error struct. If state->fatal_error is set, return DESTROY_STATE. Otherwise, return RESET_STATE.
 * </p>
 * @param state the state object
 * @return RESET_STATE or DESTROY_STATE
 */
int do_handle_error(struct state *state);

#endif //CSH_EXECUTE_H
