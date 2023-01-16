#include "../include/command.h"
#include <wordexp.h>
#include <string.h>

/**
 * expand_cmds
 * <p>
 * Expand all cmds from their condensed forms
 * </p>
 * @param cmds the cmds to expand
 */
char **expand_cmds(struct supervisor *supvis, char **cmds, size_t num_cmds);

/**
 * save_exp_cmds
 * <p>
 * Duplicate expanded path names from a wordexp_t into a char * array for use
 * elsewhere.
 * </p>
 * @param supvis the supervisor object
 * @param exp_cmds the list into which expanded path names shall be saved
 * @param current_exp_path the current number of saved expanded path names
 * @param we the wordexp_t temporarily storing expanded path names
 * @return the updated list of expanded path names
 */
char **save_exp_cmds(struct supervisor *supvis, char **exp_cmds, size_t *current_exp_path, const wordexp_t *we);

void do_separate_commands(struct supervisor *supvis, struct state *state)
{
    // do the fancy word expansions
    
    
}

char **expand_cmds(struct supervisor *supvis, char **cmds, size_t num_cmds)
{
    char      **exp_cmds;
    size_t    current_exp_path;
    wordexp_t we;
    
    exp_cmds = (char **) mm_malloc(1 * sizeof(char *), supvis->mm,
                                    __FILE__, __func__, __LINE__);
    
    current_exp_path = 0;
    for (size_t i = 0; i < num_cmds; ++i)
    {
        switch (wordexp(*(cmds + i), &we, 0))
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
        if (dc_error_has_error(supvis->err))
        {
            break;
        }
        exp_cmds = save_exp_cmds(supvis, exp_cmds, &current_exp_path, &we);
    }
    
    wordfree(&we);
    supvis->mm->mm_free(supvis->mm, cmds);
    
    return exp_cmds;
}

char **save_exp_cmds(struct supervisor *supvis, char **exp_cmds, size_t *current_exp_path, const wordexp_t *we)
{
    *(current_exp_path) += we->we_wordc;
    exp_cmds = (char **) mm_realloc(exp_cmds, *(current_exp_path) * sizeof(char *),
                                     supvis->mm,
                                     __FILE__, __func__, __LINE__);
    
    /* Start at the last expanded path + 1th index and the 0th wordv index
     * Go until all the strings in wordv have been copied into exp_cmds. */
    if (errno)
    {
        DC_ERROR_RAISE_ERRNO(supvis->err, errno);
    }
    for (size_t path_index = (*(current_exp_path) - we->we_wordc), wordv_index = 0;
         path_index < *(current_exp_path) && wordv_index < we->we_wordc; // These two values will be the same
         ++path_index, ++wordv_index)
    {
        *(exp_cmds + path_index) = strdup(*(we->we_wordv + wordv_index));
    }
    
    return exp_cmds;
}
