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

void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state)
{

}

void display_state(const struct dc_env *env, const struct state *state, FILE *stream)
{

}

char *state_to_string(const struct dc_env *env, const struct state *state)
{

}
