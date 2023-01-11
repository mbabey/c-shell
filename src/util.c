#include "../include/util.h"
#include "../include/command.h"
#include <dc_c/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <wordexp.h>
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

/**
 * count_char_in_string
 * <p>
 * Count the number of occurrences of a character in a string.
 * </p>
 * @param c the char of which to find occurrences
 * @param str the string in which to find occurrences
 * @return the number of occurrences of c in str
 */
size_t count_char_in_string(char c, char *str);

/**
 * expand_paths
 * <p>
 * Expand all paths from their condensed forms
 * </p>
 * @param paths the paths to expand
 */
char **expand_paths(struct supervisor *supvis, char **paths, size_t num_paths);

char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths);

void save_exp_paths(struct supervisor *supvis, char ***exp_paths, size_t *current_exp_path, const wordexp_t *we);

char *get_prompt(struct supervisor *supvis)
{
    char *prompt;
    
    if ((prompt = dc_getenv(supvis->env, "PS1")) == NULL)
    {
        prompt = strdup("$ ");
    }
    
    return prompt;
}

char *get_path(struct supervisor *supvis)
{
    char *path;
    
    path = dc_getenv(supvis->env, "PATH");
    if (path == NULL)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, ENODATA);
    }
    
    return path;
}

char **parse_path(struct supervisor *supvis, const char *path_str)
{
    char   **paths;
    char   *path_str_dup;
    size_t num_paths;
    
    path_str_dup = strdup(path_str); // mem alloc here
    num_paths    = count_char_in_string(':', path_str_dup) + 1;
    
    // tokenize the path string
    paths = tokenize_path(supvis, path_str_dup, num_paths);
    
    // expand each path string
    expand_paths(supvis, paths, num_paths);
    
    // return a pointer to the first item in the list of tokens
    
    return paths;
}

char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths)
{
    char **paths;
    
    paths = mm_malloc(num_paths * sizeof(char *), supvis->mm,
                      __FILE__, __func__, __LINE__); // mem alloc here
    
    path_str_dup = dc_strtok(supvis->env, path_str_dup, ":");
    *paths = path_str_dup;
    for (size_t i = 1; path_str_dup; ++i)
    {
        path_str_dup = dc_strtok(supvis->env, NULL, ":");
        if (path_str_dup)
        {
            *(paths + i) = path_str_dup;
        }
    }
    
    return paths;
}

size_t count_char_in_string(char c, char *str)
{
    size_t occurrences;
    
    occurrences = 0;
    for (char *path_iter = str; *path_iter; ++path_iter)
    {
        if (*path_iter == c)
        {
            ++occurrences;
        }
    }
    return occurrences;
}

char **expand_paths(struct supervisor *supvis, char **paths, size_t num_paths)
{
    char      **exp_paths;
    size_t    current_exp_path;
    wordexp_t we;
    
    exp_paths = (char **) mm_malloc(1 * sizeof(char *), supvis->mm,
                                    __FILE__, __func__, __LINE__);
    
    current_exp_path = 0;
    for (size_t i = 0; i < num_paths; ++i)
    {
        switch (wordexp(*(paths + i), &we, 0))
        {
            case 0:
            {
                break;
            }
            default:
            {
                DC_ERROR_RAISE_ERRNO(supvis->err, errno);
            }
        }
        save_exp_paths(supvis, &exp_paths, &current_exp_path, &we);
    }
    
    wordfree(&we);
    
    for (size_t i = 0; i < current_exp_path; ++i)
    {
        printf("%s\n", *(exp_paths + i));
    }
    
    return exp_paths;
}

void save_exp_paths(struct supervisor *supvis, char ***exp_paths, size_t *current_exp_path, const wordexp_t *we)
{
    *(current_exp_path) += we->we_wordc;
    *(exp_paths) = (char **) mm_realloc(*(exp_paths), *(current_exp_path) * sizeof(char *),
                                        supvis->mm,
                                        __FILE__, __func__, __LINE__);
    
    /* Start at the last expanded path + 1th index and the 0th wordv index
     * Go until all the strings in wordv have been copied into *exp_paths. */
    for (size_t path_index = (*(current_exp_path) - we->we_wordc), wordv_index = 0;
         path_index < *(current_exp_path) && wordv_index < we->we_wordc; // These two values will be the same
         ++path_index, ++wordv_index)
    {
        *(*exp_paths + path_index) = strdup(*(we->we_wordv + wordv_index));
    }
}

void do_reset_state(struct supervisor *supvis, struct state *state)
{
    dc_free(supvis->env, state->current_line);
    state->current_line_length = 0;
    state->fatal_error         = false;
    do_reset_command(supvis, state->command);
    dc_free(supvis->env, state->command);
    
    dc_error_reset(supvis->err);
}

void do_reset_command(struct supervisor *supvis, struct command *command)
{

}

void display_state(struct supervisor *supvis, const struct state *state, FILE *stream)
{
    char *state_str;
    
    state_str = state_to_string(supvis, state);
    fprintf(stream, "%s", state_str);
    free(state_str);
}

char *state_to_string(struct supervisor *supvis, const struct state *state)
{
    size_t len;
    char   *line;
    
    /* Get the length of the line to print. */
    len = (state->current_line == NULL) ? strlen("Current line: NULL\n") : strlen("Current line: \n");
    len += (state->fatal_error) ? strlen("Fatal error: true\n") : strlen("Fatal error: false\n");
    
    line = dc_malloc(supvis->env, supvis->err, len + state->current_line_length + 1);
    
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
