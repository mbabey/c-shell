#include "../include/tests.h"
#include "../../include/command.h"
#include "../../include/builtins.h"
#include "../../include/supervisor.h"
#include "../../include/util.h"
#include <dc_util/strings.h>
#include <dc_util/filesystem.h>

// NOLINTBEGIN

static void
test_builtin_cd(const char *line, const char *cmd, const size_t argc, char **argv, const char *expected_dir);

Describe(builtin);

static struct supervisor     *supvis;
static struct dc_env         *environ;
static struct dc_error       *error;
static struct memory_manager *mm;

BeforeEach(builtin)
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

AfterEach(builtin)
{
    supvis->mm->mm_free_all(supvis->mm);
    free(supvis);
}

Ensure(builtin, builtin_cd)
{
    char **argv;
    
    //  cd
    //  cd ~
    //  cd ..
    //  cd /
    //  cd /path
    //  cd file.txt
    //  cd <no real dir>
    //  cd <no permission dir>
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    test_builtin_cd("cd /\n", "cd", 1, argv, "/");
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    test_builtin_cd("cd /\n", "cd", 1, argv, "/");
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    test_builtin_cd("cd /\n", "cd", 1, argv, "/");
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, "/", NULL);
    test_builtin_cd("cd /\n", "cd", 1, argv, "/");
    dc_strs_destroy_array(supvis->env, 2, argv);
}

static void test_builtin_cd(const char *line, const char *cmd,
                            const size_t argc, char **argv, const char *expected_dir)
{
    struct command command;
    char           *working_dir;
    
    
    memset(&command, 0, sizeof(struct command));
    command.line    = strdup(line);
    command.command = strdup(cmd);
    command.argc    = argc;
    command.argv    = argv;
    
    builtin_cd(supvis, &command);
    working_dir = dc_get_working_dir(supvis->env, supvis->err);
    
    assert_false(dc_error_has_error(supvis->err));
    assert_that(working_dir, is_equal_to_string(expected_dir));
    
    do_reset_command(supvis, &command);
}

TestSuite *builtin_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, builtin, builtin_cd);
    
    return suite;
}


// NOLINTEND
