#include "../include/command.h"
#include <wordexp.h>
#include <string.h>

/**
 * remove_io_from_line
 * <p>
 * Using regex, remove the IO (ie. >, >>, <, 2> ,2>>) from a command->line.
 * </p>
 * @param supvis the supervisor object
 * @param line the line to clean
 * @return the clean line
 */
char *remove_io_from_line(struct supervisor *supvis, char *line);

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
char **save_wordv_to_argv(char **wordv, char **argv, size_t argc);

/**
 * get_io_filename
 * <p>
 * Get the filename for the IO file and whether the file should be overwritten.
 * </p>
 * @param supvis the supervisor object
 * @param regex the regex to compare against the line
 * @param line the line
 * @param overwrite whether to overwrite the contents of the file
 * @return the name of the IO file
 */
char *get_io_filename(struct supervisor *supvis, regex_t *regex, const char *line, bool *overwrite);

void do_separate_commands(struct supervisor *supvis, struct state *state)
{
    struct command *command;
    
    command = mm_calloc(1, sizeof(struct command), supvis->mm,
                        __FILE__, __func__, __LINE__);
    
    if (!command)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        state->fatal_error = true;
        return;
    }
    
    command->line = strdup(state->current_line);
    supvis->mm->mm_add(supvis->mm, command->line);
    
    state->command = command;
}

char *remove_io_from_line(struct supervisor *supvis, char *line)
{
    regex_t    regex_cmd;
    regmatch_t regmatch[2];
    int        status;
    
    status = regcomp(&regex_cmd, "([^<>]*).*", REG_EXTENDED);
    
    
    
    return line;
}

void do_parse_commands(struct supervisor *supvis, struct state *state)
{
    // call parse_command for each command (there is only one in the current implementation).
    parse_command(supvis, state, state->command);
}

void parse_command(struct supervisor *supvis, struct state *state, struct command *command)
{
    command->argv        = expand_cmds(supvis, command->line, &command->argc);
    command->stdin_file  = get_io_filename(supvis, state->in_redirect_regex, command->line, NULL);
    command->stdout_file = get_io_filename(supvis, state->out_redirect_regex, command->line,
                                           &command->stdout_overwrite);
    command->stderr_file = get_io_filename(supvis, state->err_redirect_regex, command->line,
                                           &command->stderr_overwrite);
}

char *get_io_filename(struct supervisor *supvis, regex_t *regex, const char *line, bool *overwrite)
{
    char *filename;
    
    
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
    
    argv = save_wordv_to_argv(we.we_wordv, argv, *argc);
    
    wordfree(&we);
    
    return argv;
}

char **save_wordv_to_argv(char **wordv, char **argv, size_t argc)
{
    for (size_t arg_index = 0; arg_index < argc; ++arg_index)
    {
        *(argv + arg_index) = strdup(*(wordv + arg_index));
    }
    
    return argv;
}
