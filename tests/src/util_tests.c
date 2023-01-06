#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_util/strings.h>

// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);
static void test_parse_path(const char path_str, char *dirs);

Describe(util);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(util)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(util)
{
    dc_error_reset(error);
}

Ensure(util, get_path)
{

}

TestSuite *util_tests(void)
{
    return NULL;
}


// NOLINTEND
