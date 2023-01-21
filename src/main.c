#include "../include/supervisor.h"
#include "../include/shell_impl.h"
#include <stdlib.h>

int main(void)
{
    struct supervisor *supvis = init_supervisor();
    struct state *state;
    
    state = NULL;
    
    init_state(supvis, state);

    destroy_supervisor(supvis);

    return EXIT_SUCCESS;
}
