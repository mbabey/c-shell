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
 * @param ostream the steam onto which errors shall be printed
 */
int builtin_cd(struct supervisor *supvis, struct command *command, FILE *ostream);
// TODO: call chdir
// TODO: check the errno: what is the error? print an appropriate message

#endif //CSH_BUILTINS_H
