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

static void test_run_shell(const char *in, const char *expected_out, const char *expected_err, int expected_return);

Describe(shell);

static struct supervisor     *supvis;
static struct dc_env         *environ;
static struct dc_error       *error;
static struct memory_manager *mm;

BeforeEach(shell)
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

AfterEach(shell)
{
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
}

Ensure(shell, run_shell)
{
    char *dir;
    char str[BUFSIZ];
    
    dir = dc_get_working_dir(supvis->env, supvis->err);
    
    dc_setenv(supvis->env, supvis->err, "PS1", ">>>", true);
    sprintf(str, "[%s] >>>\n", dir);
    test_run_shell("exit\n", dir, "", 0);
    
    dc_unsetenv(supvis->env, supvis->err, "PS1");
    sprintf(str, "[%s] $ \n", dir);
    test_run_shell("exit\n", dir, "", 0);
    
    sprintf(str, "[%s] $ \n[/] $ \n", dir);
    test_run_shell("cd /\nexit\n", dir, "", 0);
//    test_run_shell("\n", "", "[User/mud/cProjects/projects/cshell/cmake-build-debug/src] $ \n[/] $ ", 0);
    

}

static void test_run_shell(const char *in, const char *expected_out, const char *expected_err, int expected_return)
{
    FILE *in_file;
    FILE *out_file;
    FILE *err_file;
    char out_buf[BUFSIZ];
    char err_buf[BUFSIZ];
    int ret_val;
    
    memset(out_buf, 0, sizeof(out_buf));
    memset(err_buf, 0, sizeof(err_buf));
    in_file = fmemopen(in, strlen(in) + 1, "r");
    out_file = fmemopen(out_buf, sizeof(out_buf), "w");
    err_file = fmemopen(out_buf, sizeof(err_buf), "w");
    
//    run_shell(supvis, NULL, NULL, NULL);
    ret_val = run_shell(supvis, in_file, out_file, err_file);
    assert_that(ret_val, is_not_equal_to(expected_return));
    assert_that(out_buf, is_equal_to_string(expected_out));
    assert_that(err_buf, is_equal_to_string(expected_err));
    
    fclose(in_file);
    fclose(out_file);
    fclose(err_file);
}

TestSuite *shell_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, shell, run_shell);
    
    return suite;
}
