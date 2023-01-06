//
// Created by Maxwell Babey on 1/6/23.
//

#ifndef CSHELL_TESTS_UTIL_H
#define CSHELL_TESTS_UTIL_H

#include <dc_env/env.h>
#include <dc_error/error.h>

const char *get_prompt(const struct dc_env *env, struct dc_error *err);

char *get_path(const struct dc_env *env, struct dc_error *err);

#endif //CSHELL_TESTS_UTIL_H
