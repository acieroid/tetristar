#include "game.h"

static gboolean started = FALSE;

void tetris_game_start()
{
  started = TRUE;
}

void tetris_game_stop()
{
  started = FALSE;
}

gboolean tetris_game_is_started()
{
  return started;
}

void tetris_game_reset()
{
  /* TODO */
}

