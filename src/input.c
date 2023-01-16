#include "../include/input.h"

char *read_command_line(struct supervisor *supvis, FILE *istream, FILE *ostream, size_t *line_size)
{
    char *in_buf;
    
    in_buf = (char *) mm_calloc(*line_size, sizeof(char), supvis->mm, __FILE__, __func__, __LINE__);
    
    istream = fmemopen(in_buf, *line_size, "w");
    
    
    
}
