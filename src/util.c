#include <string.h>
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
    size_t len;
    char *line;
    
    len = strlen("Current line: \n") +
            strlen("Current line length: \n") +
            strlen("Current command: \n") +
            strlen("Fatal error: \n");
    len += state->current_line_length + state->command->;
    
    sprintf(line, "Current line: %s\n"
                  "Current line length: %zu\n"
                  "Current command: %s\n"
                  "Fatal error: %d\n", , len,)
    
    
    return NULL;
}
