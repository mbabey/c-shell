#ifndef CSH_STATE_H
#define CSH_STATE_H

#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * struct state
 * <p>
 * The current FSM state, passed around to the FSM functions
 * <p>
 */
struct state
{
    /* Permanent settings */
    regex_t *in_redirect_regex;     // stdin regex
    regex_t *out_redirect_regex;    // stdout regex
    regex_t *err_redirect_regex;    // stderr regex
    char **path;                    // tokenized path
    char *prompt;                   // prompt to display before a command is entered
    size_t max_line_length;         // largest possible line
   
    /* Impermanent settings */
    char *current_line;             // line most recently entered
    size_t current_line_length;     // len of most recent line
    struct command *command;        // the commands to execute (current only)
    bool fatal_error;               // whether a fatal error has occurred
};

#endif //CSH_STATE_H
