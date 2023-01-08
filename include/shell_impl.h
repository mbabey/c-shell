//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_SHELL_IMPL_H
#define CSH_SHELL_IMPL_H

#include <dc_error/error.h>
#include <dc_env/env.h>

/**
 * init_state
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
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return READ_COMMANDS or INIT_ERROR
 */
int init_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * destroy_state
 * <p>
 * Free any dynamically allocated memory in the state.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return DC_FSM_EXIT
 */
int destroy_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * reset_state
 * <p>
 * Reset the state for the next read.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return READ_COMMANDS
 */
int reset_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * read_commands
 * <p>
 * Prompt the user and read the command line. Sets state->current_line and
 * current_line_length.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return SEPARATE_COMMANDS
 */
int read_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * separate_commands
 * <p>
 * Separate the commands. In the current implementation there is only one
 * command. Sets state->command
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return PARSE_COMMANDS or SEPARATE_ERROR
 */
int separate_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * parse_commands
 * <p>
 * Parse the commands (see parse_command).
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return EXECUTE_COMMANDS or PARSE_ERROR
 */
int parse_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * execute_commands
 * <p>
 * Run the command (see execute).
 * If the command->command is "cd", run builtin_cd.
 * If the command->command is
 * </p>
 * @param env the environment
 * @param err the error object
 * @param arg the current struct state
 * @return EXECUTE_COMMANDS or PARSE_ERROR
 */
int execute_commands(const struct dc_env *env, struct dc_error *err, void *arg);

#endif //CSH_SHELL_IMPL_H
