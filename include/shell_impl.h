#ifndef CSH_SHELL_IMPL_H
#define CSH_SHELL_IMPL_H

#include "supervisor.h"

/**
 * init_state
 * <p>
 * Initialize the state with its default values.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return READ_COMMANDS or ERROR
 */
int init_state(struct supervisor *supvis, void *arg);

/**
 * read_commands
 * <p>
 * Prompt the user and read the command line. Sets state->current_line and
 * current_line_length.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return RESET_STATE or SEPARATE_COMMANDS or ERROR
 */
int read_commands(struct supervisor *supvis, void *arg);

/**
 * separate_commands
 * <p>
 * Separate the commands.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return PARSE_COMMANDS or ERROR
 */
int separate_commands(struct supervisor *supvis, void *arg);

/**
 * parse_commands
 * <p>
 * Parse the commands (see parse_command).
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return EXECUTE_COMMANDS or ERROR
 */
int parse_commands(struct supervisor *supvis, void *arg);

/**
 * execute_commands
 * <p>
 * Run the commands.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return RESET_STATE or DESTROY_STATE or ERROR
 */
int execute_commands(struct supervisor *supvis, void *arg);

/**
 * reset_state
 * <p>
 * Reset the state for the next read.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 * @return READ_COMMANDS or ERROR
 */
int reset_state(struct supervisor *supvis, void *arg);

/**
 * handle_error
 * <p>
 * Check state->fatal_error and return the next state depending on state->fatal_error.
 * </p>
 * @param arg the current struct state
 * @return RESET_STATE or DESTROY_STATE (if state->fatal_error)
 */
int handle_error(void *arg);

/**
 * destroy_state
 * <p>
 * Reclaim memory from the state object and zero it out (NULL, 0, false).
 * This will terminate the shell.
 * </p>
 * @param supvis the supervisor object
 * @param arg the current struct state
 */
void destroy_state(struct supervisor *supvis, void *arg);

#endif //CSH_SHELL_IMPL_H
