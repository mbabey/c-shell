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
    char       *prompt;
    
    prompt = dc_getenv(environ, envname);
    if (prompt != NULL)
    {
        dc_setenv(environ, error, envname, NULL, true);
    }
    
    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("$ ")); // Test when PS1 not set
    
    dc_setenv(environ, error, envname, "ABC", true);
    
    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("ABC")); // Test when PS1 set
}

Ensure(util, get_path)
{
    const char *envname = "PATH";
    
    static const char *paths[] =
                              {
                                      "",
                                      ".",
                                      "abc",
                                      "abc:def",
                                      "/usr/bin",
                                      "/usr/bin:/usr/local/bin:/bin",
                                      ":",
                                      NULL
                              };
    
    char *env_path;
    
    dc_setenv(environ, error, envname, NULL, true);
    env_path = dc_getenv(environ, envname);
    assert_that(env_path, is_null); // Test PATH=NULL
    
    for (char *path = *paths; path != NULL; ++path) // Test PATH=<string>
    {
        dc_setenv(environ, error, envname, path, true);
        env_path = dc_getenv(environ, envname);
        assert_that(env_path, is_equal_to_string(path));
    }
}

Ensure(util, parse_path)
{

}

Ensure(util, do_reset_state)
{

}

Ensure(util, state_to_string)
{

}


TestSuite *util_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    
    add_test_with_context(suite, util, get_prompt);
    add_test_with_context(suite, util, get_path);
    add_test_with_context(suite, util, parse_path);
    add_test_with_context(suite, util, do_reset_state);
    add_test_with_context(suite, util, state_to_string);
    
    return suite;
}


// NOLINTEND
