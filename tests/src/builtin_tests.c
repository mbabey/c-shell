#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_env/env.h>
#include <dc_error/error.h>

// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);
static void test_parse_path(const char path_str, char *dirs);

Describe(builtin);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(builtin)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(builtin)
{
    dc_error_reset(error);
}

Ensure(builtin, builtin_cd)
{

}

TestSuite *builtin_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, builtin, builtin_cd);
    
    return suite;
}


// NOLINTEND
