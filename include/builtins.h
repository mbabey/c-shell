#ifndef CSH_BUILTINS_H
#define CSH_BUILTINS_H

#include "command.h"
#include "supervisor.h"

/**
 * builtin_cd
 * <p>
 * Change the working directory. ~ and no arguments are converted into
 * the user's home directory.
 * </p>
 * @param command the command structure
 * @param ostream the steam onto which errors shall be printed
 * @return 0 on success, -1 on failure
 */
int builtin_cd(struct command *command, FILE *ostream);

#endif //CSH_BUILTINS_H
