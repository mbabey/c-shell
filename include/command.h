#ifndef CSH_COMMAND_H
#define CSH_COMMAND_H

#include "state.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * struct command
 * <p>
 * Stores information about the command to be executed.
 * </p>
 */
struct command
{
    char *line;             // current command line
    char *command;          // current command
    size_t argc;            // the number of command arguments
    char **argv;            // the command arguments
    char *stdin_file;       // file from which to redirect stdin
    char *stdout_file;      // file to which to redirect stdout
    bool stdout_overwrite;  // whether to overwrite the stdout file (vs. append)
    char *stderr_file;      // file to which to redirect stderr
    bool stderr_overwrite;  // whether to overwite the stderr file (vs. append)
    int exit_code;          // the exit code from the program/builtin
};

/**
 * parse_command
 * <p>
 * Parse the command by using the command->line to fill the rest of the command fields.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param state the state object
 * @param command the command object
 */
void parse_command(const struct dc_env *env, struct dc_error *err,
        struct state *state, struct command *command);

/**
 * do_reset_command
 * <p>
 * Reset the command struct by freeing dynamically allocated memory and clearing variables.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param command the command object to reset
 */
void do_reset_command(const struct dc_env *env, struct dc_error *err, struct command *command);

#endif //CSH_COMMAND_H
