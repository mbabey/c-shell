#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_posix/dc_stdlib.h>
#include <dc_c/dc_stdlib.h>


// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);

static void test_parse_path(const char path_str, char *dirs);

Describe(util);

static struct dc_error *error;
static struct dc_env   *environ;

BeforeEach(util)
{
    error   = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(util)
{
    dc_error_reset(error);
}

Ensure(util, get_prompt)
{
    const char *envname = "PS1";
    const char *envval  = NULL;
    char       *prompt;
    
    prompt = dc_getenv(environ, envname);
    
    if (prompt != NULL)
    {
        dc_setenv(environ, error, envname, envval, true);
    }
    
    prompt = get_prompt(environ, error);
    
    assert_that(prompt, is_equal_to_string("$"));
}

TestSuite *util_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, util, get_prompt);
    
    return suite;
}


// NOLINTEND
