#include "../include/command.h"
#include <wordexp.h>
#include <string.h>

/**
 * expand_cmds
 * <p>
 * Expand all cmds from their condensed forms
 * </p>
 * @param line the cmds to expand
 */
char **expand_cmds(struct supervisor *supvis, char *line, size_t *argc);

/**
 * save_wordv_to_argv
 * <p>
 * Duplicate expanded path names from a wordexp_t into a char * array for use
 * elsewhere.
 * </p>
 * @param supvis the supervisor object
 * @param argv the list into which expanded path names shall be saved
 * @param exp_cmd_index the current number of saved expanded path names
 * @param wordv the wordexp_t temporarily storing expanded path names
 * @return the updated list of expanded path names
 */
char **save_wordv_to_argv(struct supervisor *supvis, char **wordv, char **argv, size_t argc);

void do_separate_commands(struct supervisor *supvis, struct state *state)
{
    struct command *command;
    
    command = mm_calloc(1, sizeof(struct command), supvis->mm,
                        __FILE__, __func__, __LINE__);
    
    if (!command)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        state->fatal_error = true; // A memory allocation failure is a fatal error.
        return;
    }
    
    command->line = strdup(state->current_line);
    
    state->command = command;
}

void do_parse_commands(struct supervisor *supvis, struct state *state)
{
    // call parse_command for each command (there is only one in the current implementation.
    parse_command(supvis, state, state->command);
}

void parse_command(struct supervisor *supvis, struct state *state, struct command *command)
{
    command->argv = expand_cmds(supvis, command->line, &command->argc);
}

char **expand_cmds(struct supervisor *supvis, char *line, size_t *argc)
{
    char      **argv;
    wordexp_t we;
    
    switch (wordexp(line, &we, 0))
    {
        case 0:
        {
            break;
        }
        default:
        {
            DC_ERROR_RAISE_ERRNO(supvis->err, errno);
            return NULL;
        }
    }
    
    *argc = we.we_wordc;
    argv = (char **) mm_malloc(*argc * sizeof(char *), supvis->mm,
                               __FILE__, __func__, __LINE__);
    
    argv = save_wordv_to_argv(supvis, we.we_wordv, argv, *argc);
    
    wordfree(&we);
    
    return argv;
}

char **save_wordv_to_argv(struct supervisor *supvis, char **wordv, char **argv, size_t argc)
{
    for (size_t arg_index = 0; arg_index < argc; ++arg_index)
    {
        *(argv + arg_index) = strdup(*(wordv + arg_index));
    }
    
    return argv;
}
