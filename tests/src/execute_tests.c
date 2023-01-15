#include <dc_util/strings.h>
#include "../include/tests.h"
#include "../../include/execute.h"
#include "../../include/supervisor.h"
// NOLINTBEGIN

static void test_execute(const char *cmd, char **path, char **argv, int expected_exit_code, char *in_buf, char *out_buf,
                         char *err_buf, size_t out_buf_size, size_t err_buf_size);

Describe(execute);

static struct supervisor     *supvis;
static struct dc_env         *environ;
static struct dc_error       *error;
static struct memory_manager *mm;

BeforeEach(execute)
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

AfterEach(execute)
{
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
}

Ensure(execute, execute)
{
    char **path;
    char **argv;
    
    path = dc_strs_to_array(supvis->env, supvis->err, 3, "/bin", "/usr/bin", NULL);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("pwd", path, NULL, 0, NULL, NULL, NULL, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("ls", path, NULL, 0, NULL, NULL, NULL, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    dc_strs_destroy_array(supvis->env, 3, path);
    path = dc_strs_to_array(supvis->env, supvis->err, 1, NULL);
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("ls", path, NULL, 127, NULL, NULL, NULL, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    dc_strs_destroy_array(supvis->env, 1, path);
    
    dc_strs_destroy_array(supvis->env, 3, path);
    path = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("ls", path, NULL, 127, NULL, NULL, NULL, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    dc_strs_destroy_array(supvis->env, 2, path);
}

static void test_execute(const char *cmd, char **path, char **argv, int expected_exit_code,
                         char *in_buf, char *out_buf, char *err_buf,
                         size_t out_buf_size, size_t err_buf_size)
{
    FILE           *in;
    FILE           *out;
    FILE           *err;
    struct command command;
    
    in  = fmemopen(in_buf, strlen(in_buf), "r");
    out = fmemopen(out_buf, out_buf_size, "w");
    err = fmemopen(err_buf, err_buf_size, "w");
    
    memset(&command, 0, sizeof(struct command));
    command.command = strdup(cmd);
    command.argv    = argv;
    execute(supvis, &command, path, in, out, err);
    assert_that(command.exit_code, is_equal_to(expected_exit_code));
    free(command.command);
}

TestSuite *execute_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, execute, execute);
    
    return suite;
}


// NOLINTEND
