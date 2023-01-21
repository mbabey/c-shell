#include "../include/supervisor.h"
#include "../include/shell_impl.h"
#include "../include/shell.h"
#include "../include/state.h"
#include <stdlib.h>

int main(void)
{
    struct supervisor *supvis = init_supervisor();
    struct state state;
    int next_state;
    
    next_state = init_state(supvis, &state);
    
    if (next_state == READ_COMMANDS)
    {
        next_state = read_commands(supvis, &state);
    } else if (next_state == ERROR)
    {
        next_state = handle_error(supvis, &state);
    }

    destroy_supervisor(supvis);

    return EXIT_SUCCESS;
}
