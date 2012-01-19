#include "player.h"

static GSList *players = NULL;

TetrisPlayer *tetris_player_new(int id)
{
  TetrisPlayer *player = g_malloc(sizeof(*player));
  player->id = id;
  player->nick = NULL;
  player->admin = FALSE;
  /** @TODO: take dimensions as parameters somewhere */
  player->matrix = tetris_matrix_new(10, 22);
  player->piece = NULL;
  player->piece_position[0] = 5;
  player->piece_position[1] = 0;
  return player;
}

void tetris_player_free(TetrisPlayer *player)
{
  if (player->nick != NULL)
    g_free(player->nick);
  tetris_matrix_free(player->matrix);
  g_free(player);
}

char *tetris_player_get_nick(TetrisPlayer *player)
{
  return player->nick;
}

void tetris_player_set_nick(TetrisPlayer *player, char *nick)
{
  if (player->nick != NULL)
    g_free(player->nick);
  player->nick = g_strdup(nick);
}

int tetris_player_get_id(TetrisPlayer *player)
{
  return player->id;
}

TetrisMatrix *tetris_player_get_matrix(TetrisPlayer *player)
{
  return player->matrix;
}

GSList *tetris_player_get_piece(TetrisPlayer *player)
{
  return player->piece;
}

void tetris_player_set_piece(TetrisPlayer *player, GSList *piece)
{
  g_slist_free_full(player->piece,
                    (GDestroyNotify) tetris_cell_info_free);
  player->piece = piece;
}

int *tetris_player_get_piece_position(TetrisPlayer *player)
{
  return player->piece_position;
}

void tetris_player_set_piece_position(TetrisPlayer *player,
                                      int position[2])
{
  player->piece_position[0] = position[0];
  player->piece_position[1] = position[1];
}

gboolean tetris_player_is_admin(TetrisPlayer *player)
{
  return player->admin;
}

void tetris_player_set_admin(TetrisPlayer *player, gboolean status)
{
  player->admin = status;
}

gboolean tetris_player_is_playing(TetrisPlayer *player)
{
  return player->playing;
}

void tetris_player_set_playing(TetrisPlayer *player, gboolean status)
{
  player->playing = status;
}


void tetris_player_add(TetrisPlayer *player)
{
  players = g_slist_prepend(players, player);
}

void tetris_player_remove(TetrisPlayer *player)
{
  players = g_slist_remove(players, player);
}

TetrisPlayer *tetris_player_find(int id)
{
  GSList *elem;
  TetrisPlayer *player;

  for (elem = players; elem != NULL; elem = elem->next) {
    player = elem->data;
    if (player->id == id)
      return player;
  }
  return NULL;
}

GSList *tetris_player_all()
{
  return players;
}

gboolean tetris_nick_is_available(char *nick)
{
  GSList *elem;
  TetrisPlayer *player;

  for (elem = players; elem != NULL; elem = elem->next) {
    player = elem->data;
    if (g_strcmp0(player->nick, nick) == 0)
      return FALSE;
  }
  return TRUE;
}
