#include "../include/tests.h"
#include "../../include/util.h"
#include <dc_env/env.h>
#include <dc_error/error.h>

// NOLINTBEGIN

static void check_state_reset(const struct dc_error error, const struct state state, FILE in, FILE out, FILE err);
static void test_parse_path(const char path_str, char *dirs);

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
    supvis->mm = mm;
    
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

}

TestSuite *builtin_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, builtin, builtin_cd);
    
    return suite;
}


// NOLINTEND
