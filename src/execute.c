#include "../include/execute.h"

void do_execute_commands(struct supervisor *supvis, struct state *state)
{
    execute(supvis, state->command, state->path);
}

void execute(struct supervisor *supvis, struct command *command, char **path)
{

}
