//
// Created by Maxwell Babey on 1/6/23.
//

#ifndef CSHELL_TESTS_UTIL_H
#define CSHELL_TESTS_UTIL_H

#include <dc_env/env.h>
#include <dc_error/error.h>

/**
 * get_prompt
 * <p>
 * Get the prompt to use.
 * </p>
 * @param env the environment
 * @param err the error object
 * @return the value of the PS1 env var or "$" if PS1 not set
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

#endif //CSHELL_TESTS_UTIL_H
