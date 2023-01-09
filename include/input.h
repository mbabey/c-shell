//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_INPUT_H
#define CSH_INPUT_H

#include <dc_env/env.h>
#include <dc_error/error.h>
#include <stdio.h>

/**
 * read_command_line
 * <p>
 * Read the command line from the user.
 * </p>
 * @param env the environment
 * @param err the error object
 * @param istream the stream from which to read (eg. stdin)
 * @param ostream the stream onto which to write (eg. stdout)
 * @param line_size the maximum characters to read
 * @return the command line that the user entered
 */
char *read_command_line(const struct dc_env *env, struct dc_error *err,
        FILE *istream, FILE* ostream, size_t *line_size);

#endif //CSH_INPUT_H
