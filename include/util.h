//
// Created by Maxwell Babey on 1/6/23.
//

#ifndef CSHELL_TESTS_UTIL_H
#define CSHELL_TESTS_UTIL_H

#include "state.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <stdio.h>

/**
 * get_prompt
 * <p>
 * Get the prompt to use.
 * </p>
 * @param env the environment
 * @param err the error object
 * @return the value of the PS1 env var or "$ " if PS1 not set
 */
const char *get_prompt(const struct dc_env *env, struct dc_error *err);

/**
 * get_path
 * <p>
 * Get the PATH environment variable.
 * </p>
 * @param env the environment
 * @param err the error object
 * @return the PATH env var
 */
char *get_path(const struct dc_env *env, struct dc_error *err);

/**
 * parse_path
 * <p>
 * Separate a path, such as the PATH env var, into separate directories.
 * Directories are separated with a ':' character. Any paths with '~'
 * are converted to the user's home directory.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param path_str the path var
 * @return the directories that make up the path
 */
char **parse_path(const struct dc_env *env, struct dc_error *err, char *path_str);

/**
 * do_reset_state
 * <p>
 * Reset the state for the next read by freeing dynamically allocated memory.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param state the state
 */
void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state);

/**
 * display_state
 * <p>
 * Display the state values to the parameter stream.
 * </p>
 * @param env the environment
 * @param state the state to display
 * @param stream the stream on which to display the state
 */
void display_state(const struct dc_env *env, const struct state *state, FILE *stream);

/**
 * state_to_string
 * <p>
 * Display the state value as a string.
 * </p>
 * @param env the environment
 * @param state the state to display
 * @return the state as a string
 */
char *state_to_string(const struct dc_env *env, const struct state *state);

#endif //CSHELL_TESTS_UTIL_H
