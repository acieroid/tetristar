#ifndef LIBTETRIS_PLAYER_H
#define LIBTETRIS_PLAYER_H

#include <string.h>
#include <assert.h>

#include "state.h"

typedef struct TetrisPlayer {
  int id;
  int admin;
  char *nick;
} TetrisPlayer;

TetrisPlayer *tetris_player_new(int id);
void tetris_player_free(TetrisPlayer *player);
char *tetris_player_get_nick(TetrisPlayer *player);
void tetris_player_set_nick(TetrisPlayer *player, char *nick);
int tetris_player_get_id(TetrisPlayer *player);
int tetris_player_is_admin(TetrisPlayer *player);
void tetris_player_set_admin(TetrisPlayer *player);

int tetris_nick_is_available(char *nick);
void tetris_player_add(TetrisPlayer *player);
void tetris_player_remove(TetrisPlayer *player);
TetrisPlayer *tetris_player_find(int id);
GSList *tetris_player_all();

#endif /* LIBTETRIS_PLAYER_H */
