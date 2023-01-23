#include "../include/command.h"
#include "../include/input.h"

#include <dc_util/filesystem.h>

/**
 * display_prompt
 * <p>
 * Display the current working directory and the state->prompt on the state->stdout.
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void display_prompt(struct supervisor *supvis, struct state *state);

/**
 * read_command_line
 * <p>
 * Read the command line from the user.
 * </p>
 * @param istream the stream from which to read (eg. stdin)
 * @param line_size the maximum number of characters to read
 * @return the command line that the user entered, or NULL on failure
 */
char *read_command_line(FILE *istream, size_t *line_size);

size_t do_read_commands(struct supervisor *supvis, struct state *state)
{
    display_prompt(supvis, state);
    
    state->current_line_length = state->max_line_length;
    
    state->current_line = read_command_line(state->stdin, &state->current_line_length);
    
    if (!state->current_line)
    {
        state->fatal_error = true;
    } else
    {
        supvis->mm->mm_add(supvis->mm, state->current_line);
    }
    
    return state->current_line_length;
}

void display_prompt(struct supervisor *supvis, struct state *state)
{
    char *cwd;
    
    cwd = dc_get_working_dir(supvis->env, supvis->err);
    
    fprintf(state->stdout, "[%s] %s", cwd, state->prompt);
}

char *read_command_line(FILE *istream, size_t *line_size)
{
    char    *line;
    size_t  len;
    ssize_t result_len;
    
    line = NULL;
    len  = (*line_size + 1);
    
    result_len = getline(&line, &len, istream);
    if (result_len == -1)
    {
        return NULL;
    }
    
    *line_size = result_len;
    return line;
}
