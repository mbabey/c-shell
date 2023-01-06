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

Ensure(shell_impl, get_path)
{

}

TestSuite *shell_impl_tests(void)
{
    return NULL;
}

// NOLINTEND
