#ifndef LIBTETRIS_STATE_H
#define LIBTETRIS_STATE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <glib.h>

#include "player.h"

typedef struct State {
  GSList *players;
} State;

void tetris_state_init();
void tetris_state_deinit();

extern State *state;

#endif /* LIBTETRIS_STATE_H */
