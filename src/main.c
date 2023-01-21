#include "../include/supervisor.h"
#include "../include/shell_impl.h"
#include "../include/shell.h"
#include "../include/state.h"
#include <stdlib.h>

int main(void)
{
    struct supervisor *supvis = init_supervisor();
    struct state      state;
    int               next_state;
    int               run;
    
    next_state = init_state(supvis, &state);
    
    run = 1;
    while (run)
    {
        switch (next_state)
        {
            case READ_COMMANDS:
            {
                next_state = read_commands(supvis, &state);
                break;
            }
            case SEPARATE_COMMANDS:
            {
                next_state = separate_commands(supvis, &state);
                break;
            }
            case PARSE_COMMANDS:
            {
                next_state = parse_commands(supvis, &state);
                break;
            }
            case EXECUTE_COMMANDS:
            {
                next_state = execute_commands(supvis, &state);
                break;
            }
            case RESET_STATE:
            {
                next_state = reset_state(supvis, &state);
                break;
            }
            case DESTROY_STATE:
            {
                next_state = destroy_state(supvis, &state);
                break;
            }
            case ERROR:
            {
                next_state = handle_error(supvis, &state);
                break;
            }
            case EXIT:
            {
                next_state = do_exit(supvis, &state);
                run = 0;
                break;
            }
            default:
            {
                run = 0; // Should never get here.
                break;
            }
        }
    }
    
    destroy_supervisor(supvis);
    
    return EXIT_SUCCESS;
}
