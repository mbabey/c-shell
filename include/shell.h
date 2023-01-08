//
// Created by Maxwell Babey on 1/8/23.
//

#ifndef CSH_SHELL_H
#define CSH_SHELL_H

#include <dc_error/error.h>
#include <dc_env/env.h>
#include <dc_fsm/fsm.h>

enum State
{
    INIT_STATE = DC_FSM_USER_START, // the initial state    2
    READ_COMMANDS,                  // accept user input    3
    SEPARATE_COMMANDS,              // separate commands    4
    PARSE_COMMANDS,                 // parse commands       5
    EXECUTE_COMMANDS,               // execute commands     6
    EXIT,                           // exit the shell       7
    RESET_STATE,                    // reset the state      8
    ERROR,                          // handle errors        9
    DESTROY_STATE                   // destroy the state    10
};

/**
 * run_shell
 * <p>
 * Run the shell FSM.
 * </p>
 * @param env the environment
 * @param err the error object
 * @return the exit code of the shell
 */
int run_shell(const struct dc_env *env, struct dc_error *err);

#endif //CSH_SHELL_H
