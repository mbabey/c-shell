#include "../include/command.h"
#include <wordexp.h>
#include <string.h>
#include <ctype.h>

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

/**
 * check_io_valid
 * <p>
 * Check whether the io command is valid. An io command is valid if it takes the form
 * "<", ">", "2>", ">>", or "2>>". In the case the io command takes the form ">>" or "2>>",
 * set the overwrite boolean to true.
 * </p>
 * @param line the line from which to read
 * @param rm_so the index at which to begin parsing the io command
 * @param overwrite whether the io command is an overwrite command
 * @return the index of the character immediately following the last '<' or '>', or 0 on a failure.
 */
size_t check_io_valid(const char *line, regoff_t rm_so, bool **overwrite);

/**
 * get_filename
 * <p>
 * Parse the filename from the given line. Increment the start pointer until a non-whitespace character.
 * Then, increment the end pointer from the start pointer until a whitespace character. Generate and
 * store a substring between those two characters as the filename. Expand the filename to an absolute
 * path.
 * </p>
 * @param supvis the supervisor object
 * @param line the line to parse
 * @param st_substr the start pointer
 * @return the substring containing the filename
 */
char *get_filename(struct supervisor *supvis, const char *line, size_t st_substr);

/**
 * substr
 * <p>
 * Get a substring from a string. Copy the characters from dest into src, starting from the
 * character at index st and ending at the character at index en.
 * </p>
 * @param dest the destination string
 * @param src the source string
 * @param st the start index
 * @param en the end index
 * @return the substring
 */
char *substr(char *dest, const char *src, size_t st, size_t en);

/**
 * expand_filename
 * <p>
 * Expand a single filename to an absolute filename.
 * </p>
 * @param supvis the supervisor
 * @param filename the filename to expand
 * @return the expanded filename
 */
void expand_filename(struct supervisor *supvis, char **filename);

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
    char       *filename;
    regmatch_t regmatch[2];
    int        status;
    
    filename = NULL;
    
    status = regexec(regex, line, 2, regmatch, 0);
    switch (status)
    {
        case 0: // success code
        {
            size_t st_substr;
            
            st_substr = check_io_valid(line, regmatch[1].rm_so, &overwrite);
            if (!st_substr)
            {
                break;
            }
    
            filename = get_filename(supvis, line, st_substr);
            if (!filename)
            {
                break;
            }
    
            break;
        }
        case REG_NOMATCH:
        {
            // No match code
            break;
        }
        default:
        {
            // other error code
            DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        }
    }
    
    return filename;
}

size_t check_io_valid(const char *line, regoff_t rm_so, bool **overwrite)
{
    size_t st_substr;
    size_t indicator_count;
    char   io_indicator;
    
    st_substr    = rm_so;
    io_indicator = (*(line + st_substr) == '2') ? *(line + ++st_substr) : *(line + st_substr);
    
    // Get the number of > or < symbols
    indicator_count = 1;
    while ((*(line + ++st_substr) == io_indicator) && indicator_count <= 2)
    {
        indicator_count++;
    }
    
    // if command looks like ">>> ..." or "<< ...", it is invalid.
    if ((io_indicator == '>' && indicator_count > 2) || (io_indicator == '<' && indicator_count > 1))
    {
        // print error: not a valid command
        return 0;
    }
    
    if (io_indicator == '>' && indicator_count == 2)
    {
        **overwrite = true;
    }
    
    return st_substr;
}

char *get_filename(struct supervisor *supvis, const char *line, size_t st_substr)
{
    char *filename;
    size_t en_substr;
    size_t len;
    
    // move the start pointer forward to the first non-whitespace character
    while (isspace(*(line + ++st_substr)));
    
    // set the end pointer to the start pointer, and move forward to the first whitespace character
    en_substr = st_substr;
    while (!isspace(*(line + ++en_substr)));
    
    // Get the filename substring.
    len      = en_substr - st_substr + 1;
    filename = (char *) mm_malloc(len, supvis->mm, __FILE__, __func__, __LINE__);
    filename = substr(filename, line, st_substr, en_substr);
    
    expand_filename(supvis, &filename);
    
    return filename;
}

char *substr(char *dest, const char *src, size_t st, size_t en)
{
    strncpy(dest, src + st, en - st);
    
    *(dest + en - st) = '\0';
    
    return dest;
}

void expand_filename(struct supervisor *supvis, char **filename)
{
    wordexp_t we;
    
    switch (wordexp(*filename, &we, 0))
    {
        case 0:
        {
            *filename = *we.we_wordv;
            break;
        }
        default:
        {
            *filename = NULL;
            DC_ERROR_RAISE_ERRNO(supvis->err, errno);
        }
    }
    
    wordfree(&we);
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
