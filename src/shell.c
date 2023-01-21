#include "../include/shell.h"
#include "../include/supervisor.h"
#include "../include/shell_impl.h"
#include "../include/state.h"

/**
 * run_shell
 * <p>
 * Run the shell FSM.
 * </p>
 * @param supvis the supervisor object
 * @param in the input stream
 * @param out the output stream
 * @param err the error stream
 * @return the exit code of the shell
 */
int run_shell(struct supervisor *supvis, FILE *in, FILE *out, FILE *err);

int run(void)
{
    struct supervisor *supvis;
    
    supvis = init_supervisor();
    
    run_shell(supvis, stdin, stdout, stderr);
    
    destroy_supervisor(supvis);
}

int run_shell(struct supervisor *supvis, FILE *in, FILE *out, FILE *err)
{
    struct state state;
    int          next_state;
    int          run;
    int exit_status;
    
    state.stdin  = in;
    state.stdout = out;
    state.stderr = err;
    
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
                run        = 0;
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

                break;
            }
            default:
            {
                run = 0; // Should never get here.
                break;
            }
        }
    }
    
    
}

