#include "../include/supervisor.h"
#include "../include/input.h"
#include "../include/command.h"
#include <dc_util/filesystem.h>

/**
 * display_prompt
 * <p>
 * Display the current working directory and the state->prompt on the state->stdout
 * </p>
 * @param supvis the supervisor object
 * @param state the state object
 */
void display_prompt(struct supervisor *supvis, struct state *state);

size_t do_read_commands(struct supervisor *supvis, struct state *state)
{
    display_prompt(supvis, state);
    
    state->current_line_length = state->max_line_length;
    
    state->current_line = read_command_line(supvis, state->stdin, NULL, &state->current_line_length);
    
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

char *read_command_line(struct supervisor *supvis, FILE *istream, FILE *ostream, size_t *line_size)
{
    char    *line;
    size_t  len;
    ssize_t result_len;
    
    line = NULL;
    len  = (*line_size + 1);
    
    result_len = getline(&line, &len, istream);
    if (result_len == -1)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        return NULL;
    }
    
    *line_size = result_len;
    return line;
}
