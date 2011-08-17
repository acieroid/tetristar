#include "player.h"

TetrisPlayer *tetris_player_new(int id)
{
  TetrisPlayer *player = malloc(sizeof(*player));
  player->id = id;
  player->nick = NULL;
  player->admin = 0;
  return player;
}

void tetris_player_free(TetrisPlayer *player)
{
  assert(player != NULL);
  if (player->nick != NULL)
    free(player->nick);
  free(player);
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
    free(player->nick);
  player->nick = strdup(nick);
}

int tetris_player_get_id(TetrisPlayer *player)
{
  assert(player != NULL);
  return player->id;
}

int tetris_is_admin(TetrisPlayer *player)
{
  assert(player != NULL);
  return player->admin;
}

void tetris_set_admin(TetrisPlayer *player)
{
  assert(player != NULL);
  player->admin = 1;
}

int tetris_nick_is_available(char *nick)
{
  GSList *elem;
  TetrisPlayer *player;

  for (elem = tetris_state_get_players(); elem != NULL; elem = elem->next) {
    player = elem->data;
    if (strcmp(player->nick, nick) == 0)
      return 0; /* nick not available */
  }
  return 1;
}

void tetris_player_add(TetrisPlayer *player)
{
  tetris_state_set_players(g_slist_prepend(tetris_state_get_players(), player));
}

void tetris_player_remove(TetrisPlayer *player)
{
  tetris_state_set_players(g_slist_remove(tetris_state_get_players(), player));
}

TetrisPlayer *tetris_player_find(int id)
{
  GSList *elem;
  TetrisPlayer *player;

  for (elem = tetris_state_get_players(); elem != NULL; elem = elem->next) {
    player = elem->data;
    if (player->id == id)
      return player;
  }
  return NULL;
}

GSList *tetris_player_all()
{
  return tetris_state_get_players();
}
