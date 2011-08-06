#include "state.h"

State *state;

void tetris_state_init()
{
  state = malloc(sizeof(*state));
  assert(state != NULL);
  state->players = NULL;
}

void tetris_state_deinit()
{
  printf("tetris_state_deinit\n");
  assert(state != NULL);
  g_slist_free_full(state->players, (GDestroyNotify) tetris_player_free);
  free(state);
}
