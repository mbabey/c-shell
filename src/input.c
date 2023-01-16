#include "../include/supervisor.h"
#include "../include/input.h"
#include "../include/command.h"

char *read_command_line(struct supervisor *supvis, FILE *istream, FILE *ostream, size_t *line_size)
{
    char   *line;
    size_t len;
    ssize_t result_len;
    
    line = NULL;
    len = (*line_size + 1);
    
    result_len = getline(&line, &len, istream);
    if (result_len == -1)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        return NULL;
    }
    
    *line_size = result_len;
    return line;
}

void do_read_commands(struct supervisor *supvis, struct state *state)
{
    state->current_line_length = state->max_line_length;
    state->current_line        = read_command_line(supvis, state->stdin, state->stdout,
                                                   &state->current_line_length);
}
