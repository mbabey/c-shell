#include "../include/tests.h"
#include "../../include/shell_impl.h"
#include "../../include/state.h"
#include "../../include/shell.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_unistd.h>
#include <stdbool.h>

// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);
static void test_parse_path(const char path_str, char *dirs);

Describe(shell_impl);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(shell_impl)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(shell_impl)
{
    dc_error_reset(error);
}

Ensure(shell_impl, init_state)
{
    struct state state;
    int next_state;
    long line_length;
    
    line_length = dc_sysconf(environ, error, _SC_ARG_MAX);
    
    assert_that_expression(line_length == _SC_ARG_MAX);
    
    dc_setenv(environ, error, "PS1", "", true);
    next_state = init_state(environ, error, &state);
    
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_that(state.in_redirect_regex, is_not_null);
    assert_that(state.out_redirect_regex, is_not_null);
    assert_that(state.err_redirect_regex, is_not_null);
    assert_that(state.prompt, is_equal_to_string("$ "));
    assert_that(state.max_line_length, is_equal_to(line_length));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
}

Ensure(shell_impl, destroy_state)
{

}

Ensure(shell_impl, reset_state)
{

}

Ensure(shell_impl, read_commands)
{

}

Ensure(shell_impl, separate_commands)
{

}

Ensure(shell_impl, parse_commands)
{

}

Ensure(shell_impl, execute_commands)
{

}

Ensure(shell_impl, do_exit)
{

}

Ensure(shell_impl, handle_error)
{

}

TestSuite *shell_impl_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, shell_impl, init_state);
    add_test_with_context(suite, shell_impl, destroy_state);
    add_test_with_context(suite, shell_impl, reset_state);
    add_test_with_context(suite, shell_impl, read_commands);
    add_test_with_context(suite, shell_impl, separate_commands);
    add_test_with_context(suite, shell_impl, parse_commands);
    add_test_with_context(suite, shell_impl, execute_commands);
    add_test_with_context(suite, shell_impl, do_exit);
    add_test_with_context(suite, shell_impl, handle_error);
    
    return suite;
}

// NOLINTEND
