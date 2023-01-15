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

}

TestSuite *shell_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, shell, run_shell);
    
    return suite;
}
