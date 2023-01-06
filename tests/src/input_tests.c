#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_util/strings.h>

// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);
static void test_parse_path(const char path_str, char *dirs);

Describe(input);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(input)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(input)
{
    dc_error_reset(error);
}

Ensure(input, read_command_line)
{

}

TestSuite *input_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, input, read_command_line);
    
    return suite;
}


// NOLINTEND
