#include "../include/command.h"
#include "../include/shell.h"
#include "../include/shell_impl.h"
#include "../include/state.h"
#include "../include/supervisor.h"

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
    int               exit_status;
    
    supvis = init_supervisor();
    
    exit_status = run_shell(supvis, stdin, stdout, stderr);
    
    destroy_supervisor(supvis);
    
    return exit_status;
}

int run_shell(struct supervisor *supvis, FILE *in, FILE *out, FILE *err)
{
    struct state state;
    int          next_state;
    int          exit_status;
    int          run;
    
    state.stdin  = in;
    state.stdout = out;
    state.stderr = err;
    
    run = 1;
    next_state = INIT_STATE;
    while (run)
    {
        switch (next_state)
        {
            case INIT_STATE:
            {
                next_state = init_state(supvis, &state);
                break;
            }
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
            case ERROR:
            {
                next_state = handle_error(&state);
                break;
            }
            case DESTROY_STATE:
            {
                exit_status = state.command->exit_code;
                destroy_state(supvis, &state);
                run        = 0;
                break;
            }
            default:
            {
                run = 0; // Should never get here.
                break;
            }
        }
    }
    
    return exit_status;
}

