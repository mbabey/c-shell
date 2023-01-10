#include "../include/util.h"
#include <stdlib.h>
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <dc_c/dc_stdlib.h>

int main(void)
{
    struct dc_env *env;
    struct dc_error *err;
    
    err = dc_error_create(true);
    env = dc_env_create(err, false, NULL);
    
    parse_path(env, err, "/usr/bin:/usr/local/bin:/bin");
    
    dc_free(env, err);
    dc_free(env, env);
    
    return EXIT_SUCCESS;
}
