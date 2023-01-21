#include "../include/util.h"
#include "../include/command.h"
#include <dc_c/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <dc_posix/dc_stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#define IN_DIRECT_REGEX "([ \t\f\v]<.*)"
#define OUT_DIRECT_REGEX "([ \t\f\v][1]?>[>]?.*)"
#define ERR_DIRECT_REGEX "([ \t\f\v]2>[>]?.*)"
#define CMD_REGEX "([^<>]*).*"

/**
 * set_regex
 * <p>
 * Set a regex_t to a pattern. If an error occurs, print an error message and return -1.
 * </p>
 * @param supvis the supervisor object
 * @param regex the regex_t to set
 * @param pattern the regex pattern
 * @param flags flags to be used
 * @return 0 on success, -1 on failure.
 */
int set_regex(struct supervisor *supvis, regex_t **regex, const char *pattern, int flags);

/**
 * set_state_regex
 * <p>
 * Set the regex patterns in the state object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @return 0 on success, -1 on error
 */
int set_state_regex(struct supervisor *supvis, struct state *state);

/**
 * set_state_path
 * <p>
 * Set the path in the provided state object.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 * @return 0 on success, -1 on failure
 */
int set_state_path(struct supervisor *supvis, struct state *state);

/**
 * tokenize_path
 * <p>
 * Given a path, separate it into individual directory paths based on the ':' character.
 * Affix a NULL pointer to the end of the list of paths.
 * </p>
 * @param supvis the supervisor object
 * @param path_str_dup the path string
 * @param num_paths the number of paths
 * @return the list of tokenized paths, or NULL if an error occurs
 */
char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths);

/**
 * free_string_array
 * <p>
 * Free all strings in a NULL-terminated array of strings and the array itself.
 * </p>
 * @param array the array of strings
 */
void free_string_array(char **array);

/**
 * bool_to_string
 * <p>
 * Convert a boolean to a string, either "true" or "false".
 * </p>
 * @param boolean the boolean to convert
 * @return the boolean as a string
 */
inline const char *bool_to_string(bool boolean);

struct state *do_init_state(struct supervisor *supvis, struct state *state)
{
//    state = mm_calloc(1, sizeof(struct state), supvis->mm,
//                      __FILE__, __func__, __LINE__);

    memset(state, 0, sizeof(struct state));

    if (state)
    {
        state->stdin           = stdin;
        state->stdout          = stdout;
        state->stderr          = stderr;
        state->max_line_length = sysconf(_SC_ARG_MAX);
        if (set_state_regex(supvis, state) == -1)
        {
            state->fatal_error = true;
            return NULL;
        }
        if (set_state_path(supvis, state) == -1)
        {
            state->fatal_error = true;
            return NULL;
        }
        state->prompt          = get_prompt(supvis);
        if (state->prompt == NULL)
        {
            state->fatal_error = true;
            return NULL;
        }
    }
    
    return state;
}

int set_state_regex(struct supervisor *supvis, struct state *state)
{
    int status;
    
    status = set_regex(supvis, &state->in_redirect_regex, IN_DIRECT_REGEX, REG_EXTENDED);
    if (status == -1)
    {
        return status;
    }
    status = set_regex(supvis, &state->out_redirect_regex, OUT_DIRECT_REGEX, REG_EXTENDED);
    if (status == -1)
    {
        regfree(state->in_redirect_regex);
        return status;
    }
    status = set_regex(supvis, &state->err_redirect_regex, ERR_DIRECT_REGEX, REG_EXTENDED);
    if (status == -1)
    {
        regfree(state->in_redirect_regex);
        regfree(state->out_redirect_regex);
        return status;
    }
    status = set_regex(supvis, &state->command_regex, CMD_REGEX, REG_EXTENDED);
    if (status == -1)
    {
        regfree(state->in_redirect_regex);
        regfree(state->out_redirect_regex);
        regfree(state->err_redirect_regex);
        return status;
    }
    
    return status;
}

int set_regex(struct supervisor *supvis, regex_t **regex, const char *pattern, int flags)
{
    int status;
    
    *regex = (regex_t *) mm_malloc(sizeof(regex_t), supvis->mm, __FILE__, __func__, __LINE__);
    
    status = regcomp(*regex, pattern, flags);
    if (status != 0)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    }
    
    return status;
}

int set_state_path(struct supervisor *supvis, struct state *state)
{
    char *path;
    
    path = get_path(supvis);
    if (!path)
    {
        return -1;
    }
    
    state->path = parse_path(supvis, path);
    if (!state->path)
    {
        return -1;
    }
    
    return 0;
}

char *get_prompt(struct supervisor *supvis)
{
    char *prompt;
    
    prompt = dc_getenv(supvis->env, "PS1");
    if (!prompt)
    {
        dc_setenv(supvis->env, supvis->err, "PS1", "$ ", true);
        prompt = dc_getenv(supvis->env, "PS1");
    }
    
    return prompt;
}

char *get_path(struct supervisor *supvis)
{
    char *path;
    
    path = dc_getenv(supvis->env, "PATH");
    if (!path)
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
    
    paths = tokenize_path(supvis, path_str_dup, num_paths);
    
    return paths;
}

char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths)
{
    char **paths;
    
    paths = mm_malloc((num_paths + 1) * sizeof(char *), supvis->mm,
                      __FILE__, __func__, __LINE__); // mem alloc here
    
    if (errno)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        return NULL;
    }
    
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
    
    *(paths + num_paths) = NULL;
    
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

void do_reset_state(struct supervisor *supvis, struct state *state)
{
    supvis->mm->mm_free(supvis->mm, state->current_line);
    state->current_line        = NULL;
    state->current_line_length = 0;
    do_reset_command(supvis, state->command);
    supvis->mm->mm_free(supvis->mm, state->command);
    state->fatal_error = false;
    
    dc_error_reset(supvis->err);
}

void do_reset_command(struct supervisor *supvis, struct command *command)
{
    supvis->mm->mm_free(supvis->mm, command->line);
    command->line = NULL;
    supvis->mm->mm_free(supvis->mm, command->command);
    command->command = NULL;
    command->argc    = 0;
    command->argv    = NULL; // TODO: have to free all of argv (attach NULL to end of argv in implementation)
    supvis->mm->mm_free(supvis->mm, command->stdin_file);
    command->stdin_file = NULL;
    supvis->mm->mm_free(supvis->mm, command->stdout_file);
    command->stdout_file      = NULL;
    command->stdout_overwrite = false;
    supvis->mm->mm_free(supvis->mm, command->stderr_file);
    command->stderr_file      = NULL;
    command->stderr_overwrite = false;
    command->exit_code        = 0;
}

void do_destroy_state(struct supervisor *supvis, struct state *state)
{
    if (state->stdin && state->stdin != stdin)
    {
        fclose(state->stdin);
        state->stdin = NULL;
    }
    if (state->stdout && state->stdout != stdout)
    {
        fclose(state->stdout);
        state->stdout = NULL;
    }
    if (state->stderr && state->stderr != stderr)
    {
        fclose(state->stderr);
        state->stderr = NULL;
    }
    
    if (state->in_redirect_regex)
    {
        regfree(state->in_redirect_regex);
        state->in_redirect_regex = NULL;
    }
    if (state->out_redirect_regex)
    {
        regfree(state->out_redirect_regex);
        state->out_redirect_regex = NULL;
    }
    if (state->err_redirect_regex)
    {
        regfree(state->err_redirect_regex);
        state->err_redirect_regex = NULL;
    }
    
    if (state->path)
    {
        free_string_array(state->path);
        state->path = NULL;
    }
    
    do_reset_state(supvis, state);
}

void free_string_array(char **array)
{
    char **head_ptr;
    
    head_ptr = array;
    
    while (array)
    {
        free(*array);
        ++array;
    }
    
    free(head_ptr);
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
