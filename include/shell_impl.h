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
 * <ul>
 * <li>in_redirect_regex: "[\t\f\v]<.*"</li>
 * <li>out_redirect_regex: "[\t\f\v][1^2]?>[>]?.*"</li>
 * <li>err_redirect_regex: "[\t\f\v]2>[>].*"</li>
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

#endif //CSH_SHELL_IMPL_H
