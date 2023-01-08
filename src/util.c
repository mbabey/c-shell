#include "../include/util.h"
#include "../include/command.h"
#include <dc_c/dc_stdlib.h>
#include <string.h>

inline const char *bool_to_string(bool boolean);

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

char *state_to_string(const struct dc_env *env, struct dc_error *err, const struct state *state)
{
    size_t len;
    char   *line;
    
    /* Get the length of the line to print. */
    len = (state->current_line == NULL) ? strlen("Current line: NULL\n") : strlen("Current line: \n");
    len += (state->fatal_error) ? strlen("Fatal error: true\n") : strlen("Fatal error: false\n");
    
    line = dc_malloc(env, err, len + 1);
    
    /* Print the line. */
    if (state->current_line == NULL)
    {
        sprintf(line, "Current line: NULL\n"
                      "Fatal error: %s\n", bool_to_string(state->fatal_error));
    } else
    {
        sprintf(line, "Current line: %s\n"
                      "Fatal error: %s\n",state->current_line, bool_to_string(state->fatal_error));
    }
    
    return line;
}

inline const char *bool_to_string(bool boolean)
{
    const char *str = (boolean) ? "true" : "false";
    return str;
}
