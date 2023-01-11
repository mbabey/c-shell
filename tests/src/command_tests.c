#include "../include/tests.h"
#include "../../include/command.h"
#include "../../include/shell_impl.h"
#include <dc_posix/dc_stdlib.h>
#include <dc_util/path.h>
#include <dc_util/strings.h>

// NOLINTBEGIN

static void test_parse_commands(const char *expected_line,
                                const char *expected_command,
                                const size_t expected_argc,
                                char **expected_argv,
                                const char *expected_stdin_file,
                                const char *expected_stdout_file,
                                const bool expected_stdout_overwrite,
                                const char *expected_stderr_file,
                                const bool expected_stderr_overwrite,
                                const int expected_exit_code);

static void expand_file_name(const char *expected_file, char **expanded_file);

Describe(command);

static struct supervisor     *supvis;
static struct dc_env         *environ;
static struct dc_error       *error;
static struct memory_manager *mm;

BeforeEach(command)
{
    supvis  = malloc(sizeof(struct supervisor));
    error   = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
    mm      = init_mem_manager();
    
    supvis->env = environ;
    supvis->err = error;
    supvis->mm  = mm;
    
    supvis->mm->mm_add(supvis->mm, error);
    supvis->mm->mm_add(supvis->mm, environ);
}

AfterEach(command)
{
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
}

Ensure(command, parse_command)
{
    char **argv;
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "hello", NULL);
    test_parse_commands("hello",
                        "hello",
                        1, argv,
                        NULL,
                        NULL, false,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "hello", NULL);
    test_parse_commands("./a.out 2>>err.txt\n",
                        "./a.out",
                        1, argv,
                        NULL,
                        NULL, false,
                        "err.txt", false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "~/cProjects/projects/c-shell/build/csh", NULL);
    test_parse_commands("~/cProjects/projects/c-shell/build/csh\n",
                        "~/cProjects/projects/c-shell/build/csh",
                        1, argv,
                        NULL,
                        NULL, false,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 4, "~/cProjects/projects/reliable-udp/server-src/build/server",
                            "-i", "192.168.0.252", NULL);
    test_parse_commands("~/cProjects/projects/reliable-udp/server-src/build/server -i 192.168.0.252\n",
                        "~/cProjects/projects/reliable-udp/server-src/build/server -i 192.168.0.252",
                        3, argv,
                        NULL,
                        NULL, false,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 4, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "./a.out", NULL);
    test_parse_commands("./a.out > out.txt\n",
                        "./a.out",
                        1, argv,
                        NULL,
                        NULL, false,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "./a.out", NULL);
    test_parse_commands("./a.out < in.txt\n",
                        "./a.out",
                        1, argv,
                        "in.txt",
                        NULL, false,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "./a.out", NULL);
    test_parse_commands("./a.out < in.txt > out.txt 2>>~/err.txt\n",
                        "./a.out",
                        1, argv,
                        "in.txt",
                        "out.txt", true,
                        "~/err.txt", false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "./a.out", NULL);
    test_parse_commands("./a.out < in.txt > ~/out.txt\n",
                        "./a.out",
                        1, argv,
                        "in.txt",
                        "~/out.txt", true,
                        NULL, false,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "./a.out", NULL);
    test_parse_commands("./a.out <     in.txt >> out.txt 2>   err.txt\n",
                        "./a.out",
                        1, argv,
                        "in.txt",
                        "out.txt", false,
                        "err.txt", true,
                        0);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 4, "./a.out", "foo", "bar", NULL);
    test_parse_commands("./a.out foo bar < in.txt > out.txt 2>>err.txt\n",
                        "./a.out",
                        3, argv,
                        "in.txt",
                        "out.txt", true,
                        "err.txt", false,
                        0);
    dc_strs_destroy_array(supvis->env, 4, argv);
}

static void test_parse_commands(const char *expected_line,
                                const char *expected_command,
                                const size_t expected_argc,
                                char **expected_argv,
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
    dc_unsetenv(supvis->env, supvis->err, "PS1");
    init_state(NULL, &state);
    
    state.command       = calloc(1, sizeof(struct command));
    state.command->line = strdup(expected_line);
    
    parse_command(supvis, &state, state.command);
    
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
    
    destroy_state(supvis, &state);
}

static void expand_file_name(const char *expected_file, char **expanded_file)
{
    if (expected_file == NULL)
    {
        (*expanded_file) = NULL;
    } else
    {
        dc_expand_path(supvis->env, supvis->err, expanded_file, expected_file);
        if (dc_error_has_error(supvis->err))
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
