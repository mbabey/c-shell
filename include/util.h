#ifndef CSHELL_TESTS_UTIL_H
#define CSHELL_TESTS_UTIL_H

#include "state.h"
#include "supervisor.h"

#include <stdio.h>

/**
 * get_prompt
 * <p>
 * Get the prompt to use.
 * </p>
 * @param supvis the supervisor object
 * @return the value of the PS1 env var or "$ " if PS1 not set
 */
char *get_prompt(struct supervisor *supvis);

/**
 * get_path
 * <p>
 * Get the PATH environment variable.
 * </p>
 * @param supvis the supervisor object
 * @return the PATH env var
 */
char *get_path(struct supervisor *supvis);

/**
 * parse_path
 * <p>
 * Separate a path, such as the PATH env var, into separate directories.
 * Directories are separated with a ':' character. Any paths with '~'
 * are converted to the user's home directory.
 * </p>
 * @param supvis the supervisor object
 * @param path_str the path var
 * @return the directories that make up the path
 */
char **parse_path(struct supervisor *supvis, const char *path_str);

/**
 * do_init_state
 * <p>
 * Initialize the state with its default values.
 * </p>
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

/**
 * count_char_in_string
 * <p>
 * Count the number of occurrences of a character in a string.
 * </p>
 * @param c the char of which to find occurrences
 * @param str the string in which to find occurrences
 * @return the number of occurrences of c in str
 */
size_t count_char_in_string(char c, char *str);

#endif //CSHELL_TESTS_UTIL_H
