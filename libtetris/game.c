#include "game.h"

static gboolean started = FALSE;
static gboolean paused = FALSE;

void tetris_game_start()
{
  started = TRUE;
  paused = FALSE;
}

void tetris_game_stop()
{
  started = FALSE;
}

gboolean tetris_game_is_started()
{
  return started;
}

void tetris_game_pause()
{
  paused = !paused;
}

gboolean tetris_game_is_paused()
{
  return paused;
}

void tetris_game_reset()
{
  /* TODO */
}

