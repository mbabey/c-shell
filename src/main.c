#include "../include/supervisor.h"
#include "../include/util.h"
#include <stdlib.h>

int main(void)
{
    struct supervisor *supvis = init_supervisor();
    
    parse_path(supvis, "~/usr/bin:./usr/local/bin:../bin");
    
    destroy_supervisor(supvis);
    
    return EXIT_SUCCESS;
}
