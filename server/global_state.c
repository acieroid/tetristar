#include "global_state.h"
#include "util.h"

GlobalState *global_state;

void global_state_init()
{
  global_state = NULL;
  global_state = malloc(sizeof(*global_state));
}

void global_state_deinit()
{
  free(global_state);
}
