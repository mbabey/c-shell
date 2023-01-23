#include "../include/command.h"

#include <ctype.h>
#include <string.h>
#include <wordexp.h>

/**
 * get_regex_substring
 * <p>
 * Get a substring based on a regex. In the case of an IO redirection, is_io should be set to true.
 * Overwrite points to a boolean that declares whether an IO redirection should be an overwrite or
 * an append.
 * </p>
 * @param supvis the supervisor object
 * @param regex the regex to compare against the line
 * @param line the line
 * @param overwrite whether to overwrite the contents of the file
 * @return the substring, or NULL on failure
 */
char *
get_regex_substring(struct supervisor *supvis, struct state *state, regex_t *regex, const char *line, bool *overwrite,
                    bool is_io);

/**
 * get_substring
 * <p>
 * Get a substring from the line. If is_io, parse as a filename; otherwise,
 * parse as a command.
 * </p>
 * @param supvis the supervisor object
 * @param substring the substring to be allocated
 * @param line the line to parse
 * @param st_substr the start of the substring
 * @param en_substr the end of the substring
 * @param overwrite whether, if is_io, to overwrite the contents of a file.
 * @param is_io whether to parse as io
 * @param out the stream on which to print error messages
 * @return the allocated substring, or NULL if the command is invalid.
 */
char *get_substring(struct supervisor *supvis, char *substring, const char *line, size_t st_substr, size_t en_substr,
                    bool is_io, bool **overwrite, FILE *out);

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
 * @param out the stream on which to print error messages
 * @return the index of the character immediately following the last '<' or '>', or 0 on a failure.
 */
size_t check_io_valid(const char *line, size_t rm_so, bool **overwrite, FILE *out);

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
 * @param st_substr the start of the substring
 * @return the substring containing the filename, or NULL on failure
 */
char *get_filename(struct supervisor *supvis, const char *line, size_t st_substr, FILE *out);

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
 * Expand a single filename to an absolute filename. If a parse error occurs, print a message to out
 * and set errno to EINVAL.
 * </p>
 * @param supvis the supervisor
 * @param filename the filename to expand
 * @return the expanded filename, or NULL on failure
 */
char *expand_filename(struct supervisor *supvis, char *filename, FILE *out);

/**
 * get_command_name
 * <p>
 * Get the command name and its arguments as a substring from the line.
 * </p>
 * @param supvis the supervisor object
 * @param line the line to parse
 * @param st_substr the start of the command substring
 * @param en_substr the end of the command substring
 * @return the command substring, or NULL on failure
 */
char *get_command_name(struct supervisor *supvis, const char *line, size_t st_substr, size_t en_substr);

/**
 * expand_cmds
 * <p>
 * Expand all cmds from their condensed forms. If a parse error occurs, print a message to out
 * and set errno to EINVAL.
 * </p>
 * @param supvis the supervisor object
 * @param line the cmds to expand
 * @param argc a pointer to variable holding the number of arguments in the command
 * @param out the stream on which to print errors
 * @return the list of expanded commands, or NULL if an error occurs
 */
char **expand_cmds(struct supervisor *supvis, const char *line, size_t *argc, FILE *out);

/**
 * save_wordv_to_argv
 * <p>
 * Duplicate expanded arguments from a wordexp_t into a char * array.
 * </p>
 * @param supvis the supervisor object
 * @param argv the list into which expanded arguments shall be saved
 * @param exp_cmd_index the number of arguments to save
 * @param wordv the wordexp_t temporarily storing expanded arguments
 * @return the list of expanded arguments
 */
char **save_wordv_to_argv(struct supervisor *supvis, char **wordv, char **argv, size_t argc);

void do_separate_commands(struct supervisor *supvis, struct state *state)
{
    struct command *command;
    
    command = mm_calloc(1, sizeof(struct command), supvis->mm,
                        __FILE__, __func__, __LINE__);
    
    if (!command)
    {
        state->fatal_error = true;
        return;
    }
    
    command->line = strdup(state->current_line);
    supvis->mm->mm_add(supvis->mm, command->line);
    
    state->command = command;
}

void do_parse_commands(struct supervisor *supvis, struct state *state)
{
    parse_command(supvis, state, state->command);
}

void parse_command(struct supervisor *supvis, struct state *state, struct command *command)
{
    command->command = get_regex_substring(supvis, state, state->command_regex, command->line,
                                           NULL, false);
    command->argv    = expand_cmds(supvis, command->command, &command->argc, state->stdout);
    supvis->mm->mm_free(supvis->mm, command->command);
    
    command->command = (command->argv) ? *command->argv : NULL;
    
    command->stdin_file = get_regex_substring(supvis, state, state->in_redirect_regex, command->line,
                                              NULL, true);
    
    command->stdout_file = get_regex_substring(supvis, state, state->out_redirect_regex, command->line,
                                               &command->stdout_overwrite, true);
    
    command->stderr_file = get_regex_substring(supvis, state, state->err_redirect_regex, command->line,
                                               &command->stderr_overwrite, true);
}

char *
get_regex_substring(struct supervisor *supvis, struct state *state, regex_t *regex, const char *line,
                    bool *overwrite, bool is_io)
{
    char       *substring;
    regmatch_t regmatch[2];
    int        status;
    
    substring = NULL;
    
    status = regexec(regex, line, 2, regmatch, 0);
    switch (status)
    {
        case 0: // success
        {
            substring = get_substring(supvis, substring, line,
                                      regmatch[1].rm_so, regmatch[1].rm_eo, is_io,
                                      &overwrite, state->stdout);
            break;
        }
        case REG_NOMATCH: // No match
        {
            break;
        }
        default: // other error
        {
            (void) fprintf(state->stderr, "csh: fatal error parsing command\n");
            errno = ENOTRECOVERABLE;
            state->fatal_error = true;
        }
    }
    
    return substring;
}

char *get_substring(struct supervisor *supvis, char *substring, const char *line, size_t st_substr, size_t en_substr,
                    bool is_io, bool **overwrite, FILE *out)
{
    if (is_io)
    {
        st_substr = check_io_valid(line, st_substr, overwrite, out);
        if (!st_substr)
        {
            return NULL; // Command is invalid
        }
        substring = get_filename(supvis, line, st_substr, out);
    } else
    {
        substring = get_command_name(supvis, line, st_substr, en_substr);
    }
    
    return substring;
}

size_t check_io_valid(const char *line, size_t rm_so, bool **overwrite, FILE *out)
{
    size_t st_substr;
    size_t indicator_count;
    char   io_indicator;
    
    // Regex matches the space character as the first character; adding one will get the first io character.
    st_substr    = rm_so + 1;
    io_indicator = (*(line + st_substr) == '2') ? *(line + ++st_substr) : *(line + st_substr); // NOLINT(bugprone-narrowing-conversions,cppcoreguidlines-narrowing-conversions): char -> char conversion
    
    // Get the number of > or < symbols
    indicator_count = 1;
    while ((*(line + ++st_substr) == io_indicator) && indicator_count <= 2)
    {
        indicator_count++;
    }
    
    // if command looks like ">>> ..." or "<< ...", it is invalid.
    if ((io_indicator == '>' && indicator_count > 2) || (io_indicator == '<' && indicator_count > 1))
    {
        (void) fprintf(out, "csh: parse error in I/O redirection near \'%c\'\n", io_indicator);
        errno = EINVAL;
        return 0;
    }
    
    if (io_indicator == '>' && indicator_count == 1)
    {
        **overwrite = true; // NOLINT(clang-analyzer-core.NullDereference): branch not accessed when **overwrite = NULL
    }
    
    return st_substr;
}

char *get_filename(struct supervisor *supvis, const char *line, size_t st_substr, FILE *out)
{
    char   *filename;
    size_t en_substr;
    size_t len;
    
    // if first char is whitespace, move the start pointer forward to the first non-whitespace character
    switch (isspace(*(line + st_substr)))
    {
        case true:
        {
            while (isspace(*(line + ++st_substr))); // NOLINT(hicpp-braces-around-statements,readability-braces-around-statements): braces not needed here
            break;
        }
        default:
        {
            // do nothing haha
        }
    }
    
    // set the end pointer to the start pointer, and move forward to the first whitespace character
    en_substr = st_substr;
    while (!isspace(*(line + ++en_substr))); // NOLINT(hicpp-braces-around-statements,readability-braces-around-statements): braces not needed here
    
    // Get the filename substring.
    len      = en_substr - st_substr + 1;
    filename = (char *) mm_malloc(len, supvis->mm, __FILE__, __func__, __LINE__);
    
    if (filename)
    {
        filename = substr(filename, line, st_substr, en_substr);
        filename = expand_filename(supvis, filename, out);
    }
    
    return filename;
}

char *substr(char *dest, const char *src, size_t st, size_t en)
{
    dest = strncpy(dest, src + st, en - st);
    
    *(dest + en - st) = '\0';
    
    return dest;
}

char *expand_filename(struct supervisor *supvis, char *filename, FILE *out)
{
    wordexp_t we;
    
    switch (wordexp(filename, &we, 0)) // NOLINT(concurrency-mt-unsafe): no threads here
    {
        case 0:
        {
            supvis->mm->mm_free(supvis->mm, filename);
            filename = strdup(*we.we_wordv);
            supvis->mm->mm_add(supvis->mm, filename);
            break;
        }
        default:
        {
            (void) fprintf(out, "csh: parse error in command near: \'%s\'\n", filename);
            supvis->mm->mm_free(supvis->mm, filename);
            errno = EINVAL;
            return NULL;
        }
    }
    
    wordfree(&we);
    
    return filename;
}

char *get_command_name(struct supervisor *supvis, const char *line, size_t st_substr, size_t en_substr)
{
    char   *substring;
    size_t len;
    
    if (*(line + en_substr - 1) == '2') // If an error io redirect, the regex will not capture the 2.
    {
        --en_substr;
    }
    
    len = en_substr - st_substr + 1;
    
    substring = (char *) mm_malloc(len, supvis->mm, __FILE__, __func__, __LINE__);
    
    if (substring)
    {
        substring = substr(substring, line, st_substr, en_substr);
    }
    
    return substring;
}

char **expand_cmds(struct supervisor *supvis, const char *line, size_t *argc, FILE *out)
{
    char      **argv;
    wordexp_t we;
    int       status;
    char      *line_no_newline;
    char      *newline_ptr;
    
    line_no_newline = strdup(line);
    supvis->mm->mm_add(supvis->mm, line_no_newline);
    
    newline_ptr = strchr(line_no_newline, '\n');
    if (newline_ptr)
    {
        *newline_ptr = '\0';
    }
    
    status = wordexp(line_no_newline, &we, 0); // NOLINT(concurrency-mt-unsafe): no threads here
    if (status)
    {
        (void) fprintf(out, "csh: parse error in command near: \'%s\'\n", line_no_newline);
        errno = EINVAL;
        return NULL;
    }
    
    supvis->mm->mm_free(supvis->mm, line_no_newline);
    
    *argc = we.we_wordc;
    argv = (char **) mm_malloc((*argc + 1) * sizeof(char *), supvis->mm,
                               __FILE__, __func__, __LINE__);
    if (argv)
    {
        argv = save_wordv_to_argv(supvis, we.we_wordv, argv, *argc);
    }
    
    wordfree(&we);
    
    return argv;
}

char **save_wordv_to_argv(struct supervisor *supvis, char **wordv, char **argv, size_t argc)
{
    for (size_t arg_index = 0; arg_index < argc; ++arg_index)
    {
        *(argv + arg_index) = strdup(*(wordv + arg_index));
        supvis->mm->mm_add(supvis->mm, *(argv + arg_index));
    }
    
    *(argv + argc) = NULL;
    
    return argv;
}
