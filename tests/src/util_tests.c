#include "../include/tests.h"
#include "../../include/util.h"
#include "../../include/command.h"
#include <dc_c/dc_stdlib.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_util/strings.h>
#include <dc_util/sysexits.h>
#include <stdbool.h>

// NOLINTBEGIN

static void check_state_reset(const struct state *state, FILE *in, FILE *out, FILE *err);

static void test_parse_path(const char *path_str, char **dirs);

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
    char *prompt;
    
    dc_unsetenv(environ, error, "PS1");
    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("$ ")); // Test when PS1 not set
    
    dc_setenv(environ, error, "PS1", "ABC", true);
    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("ABC")); // Test when PS1 set
}

Ensure(util, get_path)
{
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
    
    dc_unsetenv(environ, error, "PATH");
    env_path = dc_getenv(environ, "PATH");
    assert_that(env_path, is_null); // Test PATH=NULL
    
    for (const char *path = *paths; path != NULL; ++path) // Test PATH=<string>
    {
        dc_setenv(environ, error, "PATH", path, true);
        env_path = dc_getenv(environ, "PATH");
        assert_that(env_path, is_equal_to_string(path));
    }
}

Ensure(util, parse_path)
{
    test_parse_path("", dc_strs_to_array(environ, error, 1, NULL));
    test_parse_path("a", dc_strs_to_array(environ, error, 2, "a", NULL));
    test_parse_path("a:b", dc_strs_to_array(environ, error, 3, "a", "b", NULL));
    test_parse_path("a:bcde:f", dc_strs_to_array(environ, error, 4, "a", "bcde", "f", NULL));
    test_parse_path("a::b", dc_strs_to_array(environ, error, 3, "a", "b", NULL));
}

static void test_parse_path(const char *path_str, char **dirs)
{
    char   **path_dirs;
    size_t i;
    
    path_dirs = parse_path(environ, error, path_str);
    
    for (i = 0; *(dirs + i) && *(path_dirs + i); ++i)
    {
        assert_that(*(path_dirs + i), is_equal_to_string(*(dirs + i)));
    }
    
    assert_that(*(dirs + i), is_null);
    assert_that(*(path_dirs + i), is_null);
}

Ensure(util, do_reset_state)
{
    struct state state;
    
    state.stdin = stdin;
    state.stdout = stdout;
    state.stderr = stderr;
    state.in_redirect_regex   = NULL;
    state.out_redirect_regex  = NULL;
    state.err_redirect_regex  = NULL;
    state.path                = NULL;
    state.prompt              = NULL;
    state.max_line_length     = 0;
    state.current_line        = NULL;
    state.current_line_length = 0;
    state.command             = NULL;
    state.fatal_error         = false;
    
    do_reset_state(environ, error, &state);
    check_state_reset(&state, stdin, stdout, stderr);
    
    state.current_line        = strdup("");
    state.current_line_length = strlen(state.current_line);
    do_reset_state(environ, error, &state);
    check_state_reset(&state, stdin, stdout, stderr);
    
    state.current_line        = strdup("ls");
    state.current_line_length = strlen(state.current_line);
    state.command             = dc_calloc(environ, error, 1, sizeof(struct command));
    do_reset_state(environ, error, &state);
    check_state_reset(&state, stdin, stdout, stderr);
    
    DC_ERROR_RAISE_ERRNO(error, E2BIG);
    do_reset_state(environ, error, &state);
    check_state_reset(&state, stdin, stdout, stderr);
    
    state.fatal_error = true;
    do_reset_state(environ, error, &state);
    check_state_reset(&state, stdin, stdout, stderr);
}

static void check_state_reset(const struct state *state, FILE *in, FILE *out, FILE *err)
{
    assert_that(state->stdin, is_equal_to(in));
    assert_that(state->stdout, is_equal_to(out));
    assert_that(state->stderr, is_equal_to(err));
    assert_that(state->current_line, is_null);
    assert_that(state->current_line_length, is_equal_to(0));
    assert_that(state->command, is_null);
    assert_false(state->fatal_error);
    assert_that(dc_error_get_message(error), is_equal_to_string("*there is no error message set*"));
}

Ensure(util, state_to_string)
{
    struct state state;
    char         *state_str;
    
    state.in_redirect_regex   = NULL;
    state.out_redirect_regex  = NULL;
    state.err_redirect_regex  = NULL;
    state.path                = NULL;
    state.prompt              = NULL;
    state.max_line_length     = 0;
    state.current_line        = NULL;
    state.current_line_length = 0;
    state.command             = NULL;
    state.fatal_error         = false;
    
    state_str = state_to_string(environ, error, &state);
    assert_that(state_str, is_equal_to_string("Current line: NULL\nFatal error: false\n"));
    free(state_str);
    
    state.current_line = "";
    state_str = state_to_string(environ, error, &state);
    assert_that(state_str, is_equal_to_string("Current line: \nFatal error: false\n"));
    free(state_str);
    
    state.current_line = "hello";
    state_str = state_to_string(environ, error, &state);
    assert_that(state_str, is_equal_to_string("Current line: hello\nFatal error: false\n"));
    free(state_str);
    
    state.current_line = "world";
    state.fatal_error  = true;
    state_str = state_to_string(environ, error, &state);
    assert_that(state_str, is_equal_to_string("Current line: world\nFatal error: true\n"));
    free(state_str);
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
