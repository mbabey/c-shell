#ifndef CSH_COMMAND_H
#define CSH_COMMAND_H

#include "state.h"
#include "supervisor.h"

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
 * do_separate_commands
 * <p>
 * Given a state object with a filled current line field, construct a command structure in that state.
 * Then, separate the individual commands in the current_line and store them into the command structure.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void do_separate_commands(struct supervisor *supvis, struct state *state);

/**
 * do_parse_commands
 * <p>
 * For each command in the state, parse the command->line for the information necessary to execute
 * the command.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void do_parse_commands(struct supervisor *supvis, struct state *state);

/**
 * parse_command
 * <p>
 * Parse the command by using the command->line to fill the rest of the command fields.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @param command the command object
 */
void parse_command(struct supervisor *supvis, struct state *state, struct command *command);

#endif //CSH_COMMAND_H
