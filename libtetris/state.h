#ifndef LIBTETRIS_STATE_H
#define LIBTETRIS_STATE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>

#include <glib.h>

#include "player.h"

typedef struct State {
  pthread_mutex_t mutex;
  GSList *players;
} State;

void tetris_state_init();
void tetris_state_deinit();

GSList *tetris_state_get_players();
void tetris_state_set_players(GSList *players);

void tetris_state_lock();
void tetris_state_unlock();

#endif /* LIBTETRIS_STATE_H */
