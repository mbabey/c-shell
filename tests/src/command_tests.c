#include "../include/tests.h"
#include "../../include/command.h"
#include "../../include/shell_impl.h"
#include "../../include/util.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_util/path.h>

// NOLINTBEGIN

static void test_parse_commands(const char *expected_line,
                                const char *expected_command,
                                const size_t expected_argc,
                                const char **expected_argv,
                                const char *expected_stdin_file,
                                const char *expected_stdout_file,
                                const bool expected_stdout_overwrite,
                                const char *expected_stderr_file,
                                const bool expected_stderr_overwrite,
                                const int expected_exit_code);

static void expand_file_name(const char *expected_file, char **expanded_file);

Describe(command);

static struct dc_error *error;
static struct dc_env   *environ;

BeforeEach(command)
{
    error   = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(command)
{
    dc_error_reset(error);
}

Ensure(command, parse_command)
{
//    "./a.out foo bar < in.txt > out.txt 2>>err.txt\n
    
    test_parse_commands("hello",
                        "hello",
                        1, NULL,
                        NULL,
                        NULL, false,
                        NULL, false,
                        0);
    test_parse_commands("./a.out 2>>err.txt\n",
                        "./a.out",
                        1, NULL,
                        NULL,
                        NULL, false,
                        "err.txt", false,
                        0);
    test_parse_commands("./a.out > out.txt\n",
                        "./a.out",
                        1, NULL,
                        NULL,
                        "out.txt", true,
                        NULL, false,
                        0);
    test_parse_commands("./a.out < in.txt\n",
                        "./a.out",
                        1, NULL,
                        "in.txt",
                        NULL, false,
                        NULL, false,
                        0);
    test_parse_commands("./a.out < in.txt > out.txt 2>>~/err.txt\n",
                        "./a.out",
                        1, NULL,
                        "in.txt",
                        "out.txt", true,
                        "~/err.txt", false,
                        0);
    test_parse_commands("./a.out < in.txt > ~/out.txt\n",
                        "./a.out",
                        1, NULL,
                        "in.txt",
                        "~/out.txt", true,
                        NULL, false,
                        0);
    test_parse_commands("./a.out <     in.txt >> out.txt 2>   err.txt\n",
                        "./a.out",
                        1, NULL,
                        "in.txt",
                        "out.txt", false,
                        "err.txt", true,
                        0);
    test_parse_commands("./a.out foo bar < in.txt > out.txt 2>>err.txt\n",
                        "./a.out",
                        3, NULL,
                        "in.txt",
                        "out.txt", true,
                        "err.txt", false,
                        0);
}

static void test_parse_commands(const char *expected_line,
                                const char *expected_command,
                                const size_t expected_argc,
                                const char **expected_argv,
                                const char *expected_stdin_file,
                                const char *expected_stdout_file,
                                const bool expected_stdout_overwrite,
                                const char *expected_stderr_file,
                                const bool expected_stderr_overwrite,
                                const int expected_exit_code)
{
    struct state state;
    char         *expanded_stdin_file;
    char         *expanded_stdout_file;
    char         *expanded_stderr_file;
    
    expand_file_name(expected_stdin_file, &expanded_stdin_file);
    expand_file_name(expected_stdout_file, &expanded_stdout_file);
    expand_file_name(expected_stderr_file, &expanded_stderr_file);
    
    state.stdin  = NULL;
    state.stdout = NULL;
    state.stderr = NULL;
    dc_unsetenv(environ, error, "PS1");
    init_state(environ, error, &state);
    
    state.command       = calloc(1, sizeof(struct command));
    state.command->line = strdup(expected_line);
    
    parse_command(environ, error, &state, state.command);
    
    assert_that(state.command->line, is_equal_to_string(expected_line));
    assert_that(state.command->command, is_equal_to_string(expected_command));
    assert_that(state.command->argc, is_equal_to(expected_argc));
//    assert_that(state.command->argv, is_equal_to(expected_argv));
    assert_that(state.command->stdin_file, is_equal_to_string(expanded_stdin_file));
    assert_that(state.command->stdout_file, is_equal_to_string(expanded_stdout_file));
    assert_that(state.command->stdout_overwrite, is_equal_to(expected_stdout_overwrite));
    assert_that(state.command->stderr_file, is_equal_to_string(expanded_stderr_file));
    assert_that(state.command->stderr_overwrite, is_equal_to(expected_stderr_overwrite));
    assert_that(state.command->exit_code, is_equal_to(expected_exit_code));
    free(expanded_stdin_file);
    free(expanded_stdout_file);
    free(expanded_stderr_file);
}

static void expand_file_name(const char *expected_file, char **expanded_file)
{
    if (expected_file == NULL)
    {
        (*expanded_file) = NULL;
    } else
    {
        dc_expand_path(environ, error, expanded_file, expected_file);
        if (dc_error_has_error(error))
        {
            fail_test(expected_file);
        }
    }
}

TestSuite *command_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, command, parse_command);
    
    return suite;
}


// NOLINTEND
