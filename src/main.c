#include "../include/supervisor.h"
#include "../include/util.h"
#include <stdlib.h>

int main(void)
{
    struct supervisor *supvis = init_supervisor();

    
    
    destroy_supervisor(supvis);

    return EXIT_SUCCESS;
}
