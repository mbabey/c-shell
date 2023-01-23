#ifndef CSH_INPUT_H
#define CSH_INPUT_H

#include "state.h"
#include "supervisor.h"

#include <stdio.h>

/**
 * do_read_commands
 * <p>
 * Print the prompt onto state->stdout. Read from the state->stdin into the state->command_line.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @return the size of the state->command_line
 */
size_t do_read_commands(struct supervisor *supvis, struct state *state);

#endif //CSH_INPUT_H
