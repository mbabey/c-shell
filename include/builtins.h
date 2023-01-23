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

/**
 * builtin_which
 * <p>
 * Print to the stream specified by ostream the location of a command that exists on the path.
 * </p>
 * @param cmd the command to search for
 * @param path the path on which to search
 * @param ostream the stream on which to print the result
 * @return 0 on success, -1 on failure
 */
int builtin_which(char *cmd, char **path, FILE *ostream);

#endif //CSH_BUILTINS_H
