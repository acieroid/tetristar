#include "player.h"

static GSList *players = NULL;

TetrisPlayer *tetris_player_new(int id)
{
  TetrisPlayer *player = g_malloc(sizeof(*player));
  player->id = id;
  player->nick = NULL;
  player->admin = FALSE;
  return player;
}

void tetris_player_free(TetrisPlayer *player)
{
  assert(player != NULL);
  if (player->nick != NULL)
    g_free(player->nick);
  g_free(player);
}

char *tetris_player_get_nick(TetrisPlayer *player)
{
  assert(player != NULL);
  return player->nick;
}

void tetris_player_set_nick(TetrisPlayer *player, char *nick)
{
  assert(player != NULL);
  assert(nick != NULL);

  if (player->nick != NULL)
    g_free(player->nick);
  player->nick = g_strdup(nick);
}

int tetris_player_get_id(TetrisPlayer *player)
{
  assert(player != NULL);
  return player->id;
}

gboolean tetris_is_admin(TetrisPlayer *player)
{
  assert(player != NULL);
  return player->admin;
}

void tetris_set_admin(TetrisPlayer *player)
{
  assert(player != NULL);
  player->admin = TRUE;
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

