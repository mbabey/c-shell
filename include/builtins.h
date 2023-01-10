//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_BUILTINS_H
#define CSH_BUILTINS_H

#include "command.h"
#include "supervisor.h"

/**
 * builtin_cd
 * <p>
 * Change the working directory. ~ and no arguments are converted into
 * the user's home directory. command->exit_code is set to 0 on success,
 * or err->errno_code on failure.
 * </p>
 * @param supvis the supervisor object
 * @param command the command structure
 */
void builtin_cd(struct supervisor *supvis, struct command *command);

#endif //CSH_BUILTINS_H
