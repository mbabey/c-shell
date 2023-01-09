#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_util/strings.h>

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
