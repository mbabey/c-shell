#include "../include/util.h"

const char *get_prompt(const struct dc_env *env, struct dc_error *err)
{
    return "$ ";
}

char *get_path(const struct dc_env *env, struct dc_error *err)
{
    return NULL;
}

char **parse_path(const struct dc_env *env, struct dc_error *err, char *path_str)
{
    return NULL;
}
