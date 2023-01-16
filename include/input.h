//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_INPUT_H
#define CSH_INPUT_H

#include "supervisor.h"
#include "state.h"
#include <stdio.h>

/**
 * read_command_line
 * <p>
 * Read the command line from the user.
 * </p>
 * @param supvis the supervisor object
 * @param istream the stream from which to read (eg. stdin)
 * @param ostream the stream onto which to write (eg. stdout)
 * @param line_size the maximum characters to read
 * @return the command line that the user entered
 */
char *read_command_line(struct supervisor *supvis, FILE *istream, FILE *ostream, size_t *line_size);

/**
 * do_read_commands
 * <p>
 * Read from the state stdin into the state command line.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void do_read_commands(struct supervisor *supvis, struct state *state);

#endif //CSH_INPUT_H
