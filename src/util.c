#include "../include/command.h"
#include "../include/util.h"

#include <dc_c/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <dc_posix/dc_stdlib.h>

#include <string.h>
#include <regex.h>
#include <unistd.h>

#define IN_DIRECT_REGEX "([ \t\f\v]<.*)"
#define OUT_DIRECT_REGEX "([ \t\f\v][1]?>[>]?.*)"
#define ERR_DIRECT_REGEX "([ \t\f\v]2>[>]?.*)"
#define CMD_REGEX "([^<>]*).*"

/**
 * set_regex
 * <p>
 * Set a regex_t to a pattern.
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
 * parse_path
 * <p>
 * Separate a path, such as the PATH env var, into separate directories.
 * Directories are separated with a ':' character. Any paths with '~'
 * are converted to the user's home directory.
 * </p>
 * @param supvis the supervisor object
 * @param path_str the path environment variable
 * @return the directories that make up the path
 */
char **parse_path(struct supervisor *supvis, const char *path_str);

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
 * tokenize_path
 * <p>
 * Given a path, separate it into individual directory paths based on the ':' character.
 * Affix a NULL pointer to the end of the list of paths.
 * </p>
 * @param supvis the supervisor object
 * @param path_str_dup the path string
 * @param num_paths the number of directories on the path
 * @return the list of directories on the path, or NULL if an error occurs
 */
char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths);

/**
 * get_prompt
 * <p>
 * Get the prompt to use.
 * </p>
 * @param supvis the supervisor object
 * @return the value of the PS1 env var or "$ " if PS1 not set
 */
char *get_prompt(struct supervisor *supvis);

/**
 * free_string_array
 * <p>
 * Free all strings in a NULL-terminated array of strings and the array itself.
 * </p>
 * @param array the array of strings
 */
void free_string_array(struct supervisor *supvis, char **array);

struct state *do_init_state(struct supervisor *supvis, struct state *state)
{
    if (state)
    {
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
        
        state->prompt = get_prompt(supvis);
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
    
    return status;
}

int set_state_path(struct supervisor *supvis, struct state *state)
{
    char *path;
    
    path = dc_getenv(supvis->env, "PATH");
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

char **parse_path(struct supervisor *supvis, const char *path_str)
{
    char   **paths;
    char   *path_str_dup;
    size_t num_paths;
    
    path_str_dup = strdup(path_str);
    supvis->mm->mm_add(supvis->mm, path_str_dup);
    
    num_paths = count_char_in_string(':', path_str_dup) + 1;
    
    paths = tokenize_path(supvis, path_str_dup, num_paths);
    
    return paths;
}

char **tokenize_path(struct supervisor *supvis, char *path_str_dup, size_t num_paths)
{
    char **paths;
    
    paths = mm_malloc((num_paths + 1) * sizeof(char *), supvis->mm,
                      __FILE__, __func__, __LINE__); // mem alloc here
    
    if (!paths)
    {
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

void do_reset_state(struct supervisor *supvis, struct state *state)
{
    
    supvis->mm->mm_free(supvis->mm, state->current_line);
    state->current_line        = NULL;
    state->current_line_length = 0;
    if (state->command)
    {
        do_reset_command(supvis, state->command);
        supvis->mm->mm_free(supvis->mm, state->command);
        state->command     = NULL;
    }
    state->fatal_error = false;
    
    dc_error_reset(supvis->err);
}

void do_reset_command(struct supervisor *supvis, struct command *command)
{
    supvis->mm->mm_free(supvis->mm, command->line);
    command->line    = NULL;
    command->command = NULL;
    command->argc    = 0;
    free_string_array(supvis, command->argv);
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
    
    if (state->command_regex)
    {
        regfree(state->command_regex);
        state->command_regex = NULL;
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
        supvis->mm->mm_free(supvis->mm, *state->path);
        *state->path = NULL;
        supvis->mm->mm_free(supvis->mm, state->path);
        state->path = NULL;
    }
    if (state->prompt)
    {
        state->prompt = NULL;
    }
    
    do_reset_state(supvis, state);
}

void free_string_array(struct supervisor *supvis, char **array)
{
    if (!array)
    {
        return;
    }
    
    char **head_ptr;
    
    head_ptr = array;
    
    while (*array)
    {
        supvis->mm->mm_free(supvis->mm, *array);
        ++array;
    }
    
    supvis->mm->mm_free(supvis->mm, head_ptr);
}
