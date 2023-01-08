//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_BUILTINS_H
#define CSH_BUILTINS_H

#include "command.h"
#include <dc_error/error.h>
#include <dc_env/env.h>

/**
 * builtin_cd
 * <p>
 * Change the working directory. ~ and no arguments are converted into
 * the user's home directory. command->exit_code is set to 0 on success,
 * or err->errno_code on failure.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param command the command structure
 */
void builtin_cd(const struct dc_env *env, struct dc_error *err, struct command *command);

#endif //CSH_BUILTINS_H
