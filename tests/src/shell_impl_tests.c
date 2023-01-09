#include "../include/tests.h"
#include "../../include/command.h"
#include "../../include/shell_impl.h"
#include "../../include/state.h"
#include "../../include/shell.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_unistd.h>
#include <dc_util/filesystem.h>
#include <stdbool.h>
#include <dc_c/dc_stdlib.h>

// NOLINTBEGIN

static void test_init_state(const char *expected_prompt, FILE *in, FILE *out, FILE *err);

static void test_destroy_state(bool initial_fatal, FILE *in, FILE *out, FILE *err);

static void test_reset_state(const char *expected_prompt, bool initial_fatal);

static void test_read_commands(const char *test_input, const char *expected_command, int expected_state);

static void test_separate_commands(const char *test_input, const char *expected_command, int read_expected_state);

static void test_parse_commands(const char *test_input);

Describe(shell_impl);

static struct dc_error *error;
static struct dc_env   *environ;

BeforeEach(shell_impl)
{
    error   = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(shell_impl)
{
    dc_error_reset(error);
}

Ensure(shell_impl, init_state)
{
    dc_unsetenv(environ, error, "PS1");
    test_init_state("$ ", stdin, stdout, stderr);
    
    dc_setenv(environ, error, "PS1", "gabagool: ", true);
    test_init_state("gabagool: ", stdin, stdout, stderr);
}

static void test_init_state(const char *expected_prompt, FILE *in, FILE *out, FILE *err)
{
    struct state state;
    int          next_state;
    long         line_length;
    
    state.stdin  = stdin;
    state.stdout = stdout;
    state.stderr = stderr;
    
    line_length = dc_sysconf(environ, error, _SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
    
    next_state = init_state(environ, error, &state);
    
    assert_false(dc_error_has_no_error(error));
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    
    assert_that(state.stdin, is_equal_to(in));
    assert_that(state.stdout, is_equal_to(out));
    assert_that(state.stderr, is_equal_to(err));
    assert_that(state.in_redirect_regex, is_not_null);
    assert_that(state.out_redirect_regex, is_not_null);
    assert_that(state.err_redirect_regex, is_not_null);
    assert_that(state.path, is_not_null);
    assert_that(state.prompt, is_equal_to_string(expected_prompt));
    assert_that(state.max_line_length, is_equal_to(line_length));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
    assert_false(state.fatal_error);
}

Ensure(shell_impl, destroy_state)
{
    test_destroy_state(true, stdin, stdout, stderr);
    test_destroy_state(false, stdin, stdout, stderr);
}

static void test_destroy_state(bool initial_fatal, FILE *in, FILE *out, FILE *err)
{
    struct state state;
    int          next_state;
    long         line_length;
    
    state.stdin  = stdin;
    state.stdout = stdout;
    state.stderr = stderr;
    
    line_length = dc_sysconf(environ, error, _SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
    
    init_state(environ, error, &state);
    next_state = destroy_state(environ, error, &state);
    
    assert_false(dc_error_has_no_error(error));
    assert_that(next_state, is_equal_to(EXIT));
    
    assert_that(state.stdin, is_equal_to(in));
    assert_that(state.stdout, is_equal_to(out));
    assert_that(state.stderr, is_equal_to(err));
    assert_that(state.in_redirect_regex, is_null);
    assert_that(state.out_redirect_regex, is_null);
    assert_that(state.err_redirect_regex, is_null);
    assert_that(state.path, is_null);
    assert_that(state.prompt, is_null);
    assert_that(state.max_line_length, is_equal_to(0));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
    state.fatal_error = initial_fatal;
    assert_false(state.fatal_error);
}

Ensure(shell_impl, reset_state)
{
    dc_unsetenv(environ, error, "PS1");
    test_reset_state("$ ", false);
    
    dc_setenv(environ, error, "PS1", "hahaha: ", true);
    test_reset_state("hahaha: ", false);
    
    dc_unsetenv(environ, error, "PS1");
    test_reset_state("$ ", true);
    
    dc_setenv(environ, error, "PS1", "hahaha: ", true);
    test_reset_state("hahaha: ", true);
}

static void test_reset_state(const char *expected_prompt, bool initial_fatal)
{
    struct state state;
    int          next_state;
    long         line_length;
    
    state.stdin  = stdin;
    state.stdout = stdout;
    state.stderr = stderr;
    
    line_length = dc_sysconf(environ, error, _SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
    
    next_state = init_state(environ, error, &state);
    
    assert_false(dc_error_has_no_error(error));
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    
    assert_that(state.stdin, is_equal_to(stdin));
    assert_that(state.stdout, is_equal_to(stdout));
    assert_that(state.stderr, is_equal_to(stderr));
    assert_that(state.in_redirect_regex, is_not_null);
    assert_that(state.out_redirect_regex, is_not_null);
    assert_that(state.err_redirect_regex, is_not_null);
    assert_that(state.path, is_not_null);
    assert_that(state.prompt, is_equal_to_string(expected_prompt));
    assert_that(state.max_line_length, is_equal_to(line_length));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
    state.fatal_error = initial_fatal;
    assert_false(state.fatal_error);
}

Ensure(shell_impl, read_commands)
{
    test_read_commands("hello\n", "hello", SEPARATE_COMMANDS);
    test_read_commands("\n", "", RESET_STATE);
    test_read_commands("hello", "hello", SEPARATE_COMMANDS);
    test_read_commands("", "", RESET_STATE);
}

static void test_read_commands(const char *test_input, const char *expected_command, int expected_state)
{
    const size_t extra_prompt_chars = 5;
    struct state state;
    FILE         *in;
    FILE         *out;
    size_t       in_size;
    char         *in_buf;
    char         out_buf[BUFSIZ];
    char         *cwd;
    char         *prompt;
    int          next_state;
    
    in_buf  = strdup(test_input);
    in_size = strlen(in_buf) + 1;
    in      = fmemopen(in_buf, in_size, "r");
    out     = fmemopen(out_buf, sizeof(out_buf), "w");
    
    state.stdin  = in;
    state.stdout = out;
    state.stderr = stderr;
    dc_unsetenv(environ, error, "PS1");
    next_state = init_state(environ, error, &state);
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    next_state = read_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(expected_state));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    cwd    = dc_get_working_dir(environ, error);
    prompt = dc_malloc(environ, error, strlen(cwd) + strlen(state.prompt) + extra_prompt_chars);
    sprintf(prompt, "[%s] %s", cwd, state.prompt);
    
    assert_that(out_buf, is_equal_to_string(prompt));
    assert_that(state.current_line, is_equal_to_string(expected_command));
    assert_that(state.current_line_length, is_equal_to(strlen(state.current_line)));
}

Ensure(shell_impl, separate_commands)
{
    test_separate_commands("./a.out\n", "./a.out", SEPARATE_COMMANDS);
    test_separate_commands("cd ~\n", "cd ~", SEPARATE_COMMANDS);
    test_separate_commands("\n", "", RESET_STATE);
}

static void test_separate_commands(const char *test_input, const char *expected_command, int read_expected_state)
{
    struct state state;
    FILE         *in;
    FILE         *out;
    size_t       in_size;
    char         *in_buf;
    char         out_buf[BUFSIZ];
    int          next_state;
    
    in_buf  = strdup(test_input);
    in_size = strlen(in_buf) + 1;
    in      = fmemopen(in_buf, in_size, "r");
    out     = fmemopen(out_buf, sizeof(out_buf), "w");
    
    state.stdin  = in;
    state.stdout = out;
    state.stderr = stderr;
    dc_unsetenv(environ, error, "PS1");
    next_state = init_state(environ, error, &state);
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    next_state = read_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(read_expected_state));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    assert_that(state.current_line, is_equal_to_string(expected_command));
    assert_that(state.current_line_length, is_equal_to(strlen(state.current_line)));
    
    if (next_state == RESET_STATE)
    {
        return;
    }
    
    next_state = separate_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(PARSE_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    assert_that(state.command, is_null);
    assert_that(state.command->line, is_equal_to_string(state.current_line)); // TODO: will need to be token later
    assert_that(state.command->line, is_not_equal_to(state.current_line));
    assert_that(state.command->command, is_null);
    assert_that(state.command->argc, is_equal_to(0));
    assert_that(state.command->argv, is_null);
    assert_that(state.command->stdin_file, is_null);
    assert_that(state.command->stdout_file, is_null);
    assert_false(state.command->stdout_overwrite);
    assert_that(state.command->stderr_file, is_null);
    assert_false(state.command->stderr_overwrite);
    assert_that(state.command->exit_code, is_equal_to(0));
    
}

Ensure(shell_impl, parse_commands)
{
    test_parse_commands("hello\n");
}

static void test_parse_commands(const char *test_input)
{
    struct state state;
    FILE         *in;
    FILE         *out;
    size_t       in_size;
    char         *in_buf;
    char         out_buf[BUFSIZ];
    int          next_state;
    
    in_buf  = strdup(test_input);
    in_size = strlen(in_buf) + 1;
    in      = fmemopen(in_buf, in_size, "r");
    out     = fmemopen(out_buf, sizeof(out_buf), "w");
    
    state.stdin  = in;
    state.stdout = out;
    state.stderr = stderr;
    dc_unsetenv(environ, error, "PS1");
    next_state = init_state(environ, error, &state);
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    next_state = read_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(SEPARATE_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    next_state = separate_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(PARSE_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    next_state = parse_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(EXECUTE_COMMANDS));
    assert_false(dc_error_has_no_error(error));
    assert_false(state.fatal_error);
    
    
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
