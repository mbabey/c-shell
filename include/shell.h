#ifndef CSH_SHELL_H
#define CSH_SHELL_H

#include "supervisor.h"

#include <dc_fsm/fsm.h>
#include <stdio.h>

/**
 * State
 * <p>
 * The states in which the shell may exist.
 * </p>
 */
enum State
{
    INIT_STATE = DC_FSM_USER_START, // the initial state    2
    READ_COMMANDS,                  // accept user input    3
    SEPARATE_COMMANDS,              // separate commands    4
    PARSE_COMMANDS,                 // parse commands       5
    EXECUTE_COMMANDS,               // execute commands     6
    RESET_STATE,                    // reset the state      7
    ERROR,                          // handle errors        8
    DESTROY_STATE                   // destroy the state    9
};

/**
 * run
 * <p>
 * Run the program.
 * </p>
 * @return the exit code of the program.
 */
int run(void);

#endif //CSH_SHELL_H
