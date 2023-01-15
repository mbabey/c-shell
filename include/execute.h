#ifndef CSH_EXECUTE_H
#define CSH_EXECUTE_H

#include "command.h"
#include "supervisor.h"

/**
 * execute
 * <p>
 * Create a child process, exec the command with any redirection, and set the exit code.
 * If there is an error executing the command, print an exit code.
 * If the command cannot be found set command->exit_code to 127.
 * </p>
 * @param supvis the supervisor object
 * @param command the command struct
 * @param path the path
 */
void execute(struct supervisor *supvis, struct command *command, char **path, FILE *in, FILE *out, FILE *err);

#endif //CSH_EXECUTE_H
