#ifndef CSHELL_TESTS_UTIL_H
#define CSHELL_TESTS_UTIL_H

#include "state.h"
#include "supervisor.h"

#include <stdio.h>

/**
 * do_init_state
 * <p>
 * Set up the initial state.
 * </p>
 *
 * <ul>
 * <li>in_redirect_regex: "[&#x2216;t&#x2216;f&#x2216;v]<.*"</li>
 * <li>out_redirect_regex: "[&#x2216;t&#x2216;f&#x2216;v][1^2]?>[>]?.*"</li>
 * <li>err_redirect_regex: "[&#x2216;t&#x2216;f&#x2216;v]2>[>].*"</li>
 * <li>path: the PATH env var separated into directories</li>
 * <li>prompt: the PS1 env var if set, otherwise "$"</li>
 * <li>max_line_length: the value of _SC_ARG_MAX (see sysconfig)</li>
 * </ul>
 * @param supvis the supervisor object
 * @param state the state to initialize
 * @return the initialized state, or NULL if an error occurs.
 */
struct state *do_init_state(struct supervisor *supvis, struct state *state);

/**
 * do_reset_state
 * <p>
 * Reset the state for the next read by freeing dynamically allocated memory
 * of impermanent settings. Reset the error object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state to reset
 */
void do_reset_state(struct supervisor *supvis, struct state *state);

/**
 * do_reset_command
 * <p>
 * Reset the command struct by freeing dynamically allocated memory and clearing variables.
 * </p>
 * @param supvis the supervisor object
 * @param command the command object to reset
 */
void do_reset_command(struct supervisor *supvis, struct command *command);

/**
 * do_destroy_state
 * <p>
 * Free all memory in a state object, then free the state object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void do_destroy_state(struct supervisor *supvis, struct state *state);

/**
 * display_state
 * <p>
 * Display the state values to the parameter stream.
 * </p>
 * @param supvis the supervisor object
 * @param state the state to display
 * @param stream the stream on which to display the state
 */
void display_state(struct supervisor *supvis, const struct state *state, FILE *stream);

/**
 * state_to_string
 * <p>
 * Display the state value as a string.
 * </p>
 * @param supvis the supervisor object
 * @param state the state to display
 * @return the state as a string
 */
char *state_to_string(struct supervisor *supvis, const struct state *state);

#endif //CSHELL_TESTS_UTIL_H
