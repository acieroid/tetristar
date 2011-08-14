#include "state.h"

static State *state;

void tetris_state_init()
{
  state = malloc(sizeof(*state));
  assert(state != NULL);
  state->players = NULL;
  pthread_mutex_init(&state->mutex, NULL);
}

void tetris_state_deinit()
{
  assert(state != NULL);
  g_slist_free_full(state->players, (GDestroyNotify) tetris_player_free);
  free(state);
}

GSList *tetris_state_get_players()
{
  assert(state != NULL);
  return state->players;
}

void tetris_state_set_players(GSList *players)
{
  assert(state != NULL);
  state->players = players;
}

void tetris_state_lock()
{
  assert(state != NULL);
  pthread_mutex_lock(&state->mutex);
}

void tetris_state_unlock()
{
  assert(state != NULL);
  pthread_mutex_unlock(&state->mutex);
}
