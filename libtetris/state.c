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
  assert(state != NULL);
  g_slist_foreach(state->players, (GFunc) tetris_player_glib_free, NULL);
  free(state);
}
