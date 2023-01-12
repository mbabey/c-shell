#include "../include/tests.h"
#include "../../include/command.h"
#include "../../include/builtins.h"
#include "../../include/supervisor.h"
#include "../../include/util.h"
#include <dc_util/strings.h>
#include <dc_util/path.h>
#include <dc_util/filesystem.h>
#include <unistd.h>

// NOLINTBEGIN

static void
test_builtin_cd(const char *line, const char *cmd, const size_t argc, char **argv, const char *expected_dir,
                const char *expected_message);

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
    char *path;
    char template[16];
    char message[BUFSIZ];
    
    //  cd ..
    //  cd /
    //  cd /path
    //  cd file.txt
    //  cd <no real dir>
    //  cd <no permission dir>
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, "/", NULL);
    test_builtin_cd("cd /\n", "cd", 1, argv, "/", NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    dc_expand_path(supvis->env, supvis->err, &path, "~");
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, path, NULL);
    test_builtin_cd("cd ~\n", "cd", 2, argv, path, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    dc_expand_path(supvis->env, supvis->err, &path, "~/");
    // remove the trailing slash.
    path[strlen(path) - 1] = '\0';
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, path, NULL);
    test_builtin_cd("cd ~/\n", "cd", 2, argv, path, NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    chdir("/");
    argv = dc_strs_to_array(supvis->env, supvis->err, 2, NULL, NULL);
    test_builtin_cd("cd /\n", "cd", 2, argv, "~", NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    chdir("/tmp");
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, "/dev/null", NULL);
    test_builtin_cd("cd /dev/null\n", "cd", 2, argv, "/tmp", "cd: not a directory: /dev/null\n");
    dc_strs_destroy_array(supvis->env, 2, argv);
    
    strcpy(template, "/tmp/fileXXXXX");
    mkstemp(template);
    rmdir(template);
    sprintf(message, "cd: %s: does not exist\n", template);
    argv = dc_strs_to_array(supvis->env, supvis->err, 3, NULL, template, NULL);
    test_builtin_cd("cd /tmp/fileXXXXX\n", "cd", 2, argv, "/tmp", NULL);
    dc_strs_destroy_array(supvis->env, 2, argv);
}

static void test_builtin_cd(const char *line, const char *cmd, const size_t argc, char **argv,
                            const char *expected_dir, const char *expected_message)
{
    struct command command;
    char message[BUFSIZ];
    char *working_dir;
    FILE *stderr_file;
    
    memset(&command, 0, sizeof(struct command));
    command.line    = strdup(line);
    command.command = strdup(cmd) ;
    command.argc    = argc;
    command.argv    = argv;
    
    memset(&message, 0, sizeof(message));
    stderr_file = fmemopen(message, sizeof(message), "w");
    
    builtin_cd(supvis, &command, stderr);
    
    if (dc_error_has_error(supvis->err))
    {
        assert_that(message, is_equal_to_string(expected_message));
        dc_error_reset(supvis->err);
    }
    
    working_dir = dc_get_working_dir(supvis->env, supvis->err);
    assert_that(working_dir, is_equal_to_string(expected_dir));
    
    fclose(stderr_file);
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
