#include <dc_util/strings.h>
#include "../include/tests.h"
#include "../../include/execute.h"
#include "../../include/supervisor.h"
#include "util.h"
#include <sys/stat.h>
#include <unistd.h>

// NOLINTBEGIN

static void
test_execute(const char *cmd, char **path, size_t argc, char **argv, bool check_exit_code, int expected_exit_code,
             const char *out_file_name, const char *err_file_name);

static void check_redirection(const char *file_name);

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
    char template[16];
    
    path = dc_strs_to_array(supvis->env, supvis->err, 3, "/bin", "/usr/bin", NULL);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("pwd", path, 1, argv, true, 0, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    strcpy(template, "/tmp/fileXXXXXX");
    test_execute("ls", path, 1, argv, true, 0, template, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, "adsafsdfasfsdsafgsdfsdadssdfad", NULL);
    strcpy(template, "/tmp/fileXXXXXX");
    test_execute("ls adsafsdfasfsdsafgsdfsdadssdfad", path, 2, argv, true, 1, NULL, template);
    dc_strs_destroy_array(supvis->env, 3, argv);
    
    dc_strs_destroy_array(supvis->env, 3, path);
    path = dc_strs_to_array(supvis->env, supvis->err, 1, NULL);
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("ls", path, 1, argv, false, ENOENT, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    dc_strs_destroy_array(supvis->env, 1, path);
    
    dc_strs_destroy_array(supvis->env, 3, path);
    path = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_execute("ls", path, 1, argv, true, 127, NULL, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    dc_strs_destroy_array(supvis->env, 2, path);
}

static void
test_execute(const char *cmd, char **path, size_t argc, char **argv, bool check_exit_code, int expected_exit_code,
             const char *out_file_name, const char *err_file_name)
{
    struct command command;
    
    memset(&command, 0, sizeof(struct command));
    command.command = strdup(cmd);
    command.argc    = argc;
    command.argv    = argv;
    
    if (out_file_name)
    {
        command.stdout_file = strdup(out_file_name);
    }
    if (err_file_name)
    {
        command.stderr_file = strdup(err_file_name);
    }
    
    execute(supvis, NULL, &command, path);
    
    if (expected_exit_code)
    {
        assert_that(command.exit_code, is_equal_to(expected_exit_code));
    }
    
    check_redirection(out_file_name);
    check_redirection(err_file_name);
    
    do_reset_command(supvis, &command);
    free(command.command);
}

static void check_redirection(const char *file_name)
{
    if (file_name)
    {
        struct stat st;
        int         status;
        
        status = stat(file_name, &st);
        assert_that(status, is_equal_to(0));
        assert_that(st.st_size, is_greater_than(0));
        
        unlink(file_name);
    }
}

TestSuite *execute_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, execute, execute);
    
    return suite;
}


// NOLINTEND
