#ifndef CSH_EXECUTE_H
#define CSH_EXECUTE_H

#include "command.h"
#include <dc_error/error.h>
#include <dc_env/env.h>

/**
 * execute
 * <p>
 * Create a child process, exec the command with any redirection, and set the exit code.
 * If there is an error executing the command, print an exit code.
 * If the command cannot be found set command->exit_code to 127.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param command the command struct
 * @param path the path
 */
void execute(const struct dc_env *env, struct dc_error *err, struct command *command, char **path);

#endif //CSH_EXECUTE_H
