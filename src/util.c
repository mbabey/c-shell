#include "../include/util.h"
#include "../include/command.h"
#include <dc_c/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <string.h>

/**
 * bool_to_string
 * <p>
 * Convert a boolean to a string, either "true" or "false".
 * </p>
 * @param boolean the boolean to convert
 * @return the boolean as a string
 */
inline const char *bool_to_string(bool boolean);

char *get_prompt(const struct dc_env *env, struct dc_error *err)
{
    char *prompt;
    
    if ((prompt = dc_getenv(env, "PS1")) == NULL)
    {
        prompt = "$ ";
    }
    
    return prompt;
}

char *get_path(const struct dc_env *env, struct dc_error *err)
{
    char *path;
    
    path = dc_getenv(env, "PATH");
    if (path == NULL)
    {
        DC_ERROR_RAISE_ERRNO(err, ENODATA);
    }
    
    return path;
}

char **parse_path(const struct dc_env *env, struct dc_error *err, const char *path_str)
{
    char *path_str_dup;
    char *tok;
    
    path_str_dup = strdup(path_str);
    
    // tokenize the path string
    tok = dc_strtok(env, path_str_dup, ":");
    while (tok)
    {
        printf("%s\n%s\n", tok, path_str_dup);
        tok = dc_strtok(env, path_str_dup, ":");
    }
    
    // wordexp the path string
    
    // return a pointer to the first item in the list of tokens
    
    return NULL;
}

void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state)
{
    dc_free(env, state->current_line);
    state->current_line_length = 0;
    state->fatal_error = false;
    do_reset_command(env, err, state->command);
    dc_free(env, state->command);
    
    dc_error_reset(err);
}

void do_reset_command(const struct dc_env *env, struct dc_error *err, struct command *command)
{

}

void display_state(const struct dc_env *env, struct dc_error *err, const struct state *state, FILE *stream)
{
    char *state_str;
    
    state_str = state_to_string(env, err, state);
    fprintf(stream, "%s", state_str);
    free(state_str);
}

char *state_to_string(const struct dc_env *env, struct dc_error *err, const struct state *state)
{
    size_t len;
    char   *line;
    
    /* Get the length of the line to print. */
    len = (state->current_line == NULL) ? strlen("Current line: NULL\n") : strlen("Current line: \n");
    len += (state->fatal_error) ? strlen("Fatal error: true\n") : strlen("Fatal error: false\n");
    
    line = dc_malloc(env, err, len + state->current_line_length + 1);
    
    /* Print the line. */
    if (state->current_line == NULL)
    {
        sprintf(line, "Current line: NULL\n"
                      "Fatal error: %s\n", bool_to_string(state->fatal_error));
    } else
    {
        sprintf(line, "Current line: %s\n"
                      "Fatal error: %s\n", state->current_line, bool_to_string(state->fatal_error));
    }
    
    return line;
}

const char *bool_to_string(bool boolean)
{
    const char *str = (boolean) ? "true" : "false";
    return str;
}
