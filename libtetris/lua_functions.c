/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lua_functions.h"

static int l_players_all(lua_State *l);
static int l_players_add(lua_State *l);
static int l_players_remove(lua_State *l);
static int l_players_get_nick(lua_State *l);
static int l_players_set_nick(lua_State *l);
static int l_players_is_admin(lua_State *l);
static int l_players_set_admin(lua_State *l);
static int l_players_is_playing(lua_State *l);
static int l_players_set_playing(lua_State *l);
static int l_players_get_piece(lua_State *l);
static int l_players_set_piece(lua_State *l);
static int l_players_get_piece_position(lua_State *l);
static int l_players_set_piece_position(lua_State *l);
static int l_players_get_next_piece(lua_State *l);
static int l_players_set_next_piece(lua_State *l);
static int l_players_get_kept_piece(lua_State *l);
static int l_players_set_kept_piece(lua_State *l);
static int l_players_add_bonus(lua_State *l);
static int l_players_remove_bonus(lua_State *l);
static int l_players_has_bonus(lua_State *l);
static int l_players_first_bonus(lua_State *l);
static int l_players_drop_bonus(lua_State *l);
static int l_players_add_points(lua_State *l);
static int l_players_reset_points(lua_State *l);
static int l_players_get_points(lua_State *l);
static int l_players_nick_valid(lua_State *l);
static int l_players_nick_available(lua_State *l);
static int l_players_exists(lua_State *l);
static int l_players_number(lua_State *l);

static int l_matrix_set_cell(lua_State *l);
static int l_matrix_get_cell(lua_State *l);
static int l_matrix_get_uncommited_cell(lua_State *l);
static int l_matrix_get_width(lua_State *l);
static int l_matrix_get_height(lua_State *l);
static int l_matrix_get_cells(lua_State *l);
static int l_matrix_diff(lua_State *l);
static int l_matrix_commit(lua_State *l);

static int l_game_start(lua_State *l);
static int l_game_stop(lua_State *l);
static int l_game_pause(lua_State *l);
static int l_game_is_paused(lua_State *l);
static int l_game_reset(lua_State *l);
static int l_game_is_started(lua_State *l);

static struct {
  gchar *category;
  /* we need to have a fixed size to compile */
  PluginFunction functions[128];
} l_functions[] = {
  { "player", {
      { "all", l_players_all },
      { "add", l_players_add },
      { "get_nick", l_players_get_nick },
      { "set_nick", l_players_set_nick },
      { "is_admin", l_players_is_admin },
      { "set_admin", l_players_set_admin },
      { "is_playing", l_players_is_playing },
      { "set_playing", l_players_set_playing },
      { "get_piece", l_players_get_piece },
      { "set_piece", l_players_set_piece },
      { "get_piece_position", l_players_get_piece_position },
      { "set_piece_position", l_players_set_piece_position },
      { "get_next_piece", l_players_get_next_piece },
      { "set_next_piece", l_players_set_next_piece },
      { "get_kept_piece", l_players_get_kept_piece },
      { "set_kept_piece", l_players_set_kept_piece },
      { "add_bonus", l_players_add_bonus },
      { "remove_bonus", l_players_remove_bonus },
      { "has_bonus", l_players_has_bonus },
      { "first_bonus", l_players_first_bonus },
      { "drop_bonus", l_players_drop_bonus },
      { "add_points", l_players_add_points },
      { "reset_points", l_players_reset_points },
      { "get_points", l_players_get_points },
      { "remove", l_players_remove },
      { "nick_valid", l_players_nick_valid },
      { "nick_available", l_players_nick_available },
      { "exists", l_players_exists },
      { "number", l_players_number },
      { NULL, NULL }
    }
  },
  { "matrix", {
      { "set_cell", l_matrix_set_cell },
      { "get_cell", l_matrix_get_cell },
      { "get_uncommited_cell", l_matrix_get_uncommited_cell },
      { "get_width", l_matrix_get_width },
      { "get_height", l_matrix_get_height },
      { "get_cells", l_matrix_get_cells },
      { "diff", l_matrix_diff },
      { "commit", l_matrix_commit },
      { NULL, NULL }
    }
  },
  { "game", {
      { "start", l_game_start },
      { "stop", l_game_stop },
      { "reset", l_game_reset },
      { "is_started", l_game_is_started },
      { "pause", l_game_pause },
      { "is_paused", l_game_is_paused },
      { NULL, NULL }
    }
  },
  { NULL, {} }
};

void tetris_lua_functions_setup(void)
{
  int i;
  for (i = 0; l_functions[i].category != NULL; i++) {
    tetris_plugin_add_category(l_functions[i].category);
    tetris_plugin_add_functions(l_functions[i].category,
                                l_functions[i].functions);
  }
}

void tetris_lua_push_fieldspec(lua_State *l, GSList *spec)
{
  int i;
  GSList *elem;
  TetrisCellInfo *info;

  CHECK_STACK_START(l);

  /* create a new table that contains the field spec */
  lua_newtable(l);

  for (elem = spec, i = 0; elem != NULL; elem = elem->next, i++) {
    info = elem->data;

    /* infos stocked as { x, y, cell } */
    lua_newtable(l);
    lua_pushnumber(l, info->x);
    lua_rawseti(l, -2, 1); /* lua indexes starts at 1 */
    lua_pushnumber(l, info->y);
    lua_rawseti(l, -2, 2);
    lua_pushnumber(l, (int) info->cell);
    lua_rawseti(l, -2, 3);

    /* add the info at the returned table */
    lua_rawseti(l, -2, i);
  }
  /* and we let the table on the stack */
  CHECK_STACK_END(l, 1);
}

GSList *tetris_lua_get_fieldspec(lua_State *l, int index)
{
  GSList *res = NULL;
  int x, y;
  TetrisCell cell;
  CHECK_STACK_START(l);

  /* iterate over the table at index */
  lua_pushnil(l);
  while (lua_next(l, index) != 0) {
    /* a table representing a cell is on the top of the stack */
#if LUA_VERSION_NUM >= 502
    if (lua_rawlen(l, -1) != 3) {
#else
    if (lua_objlen(l, -1) != 3) {
#endif
      g_warning("tetris_lua_get_fieldspec: Invalid cell");
      return res;
    }
    lua_rawgeti(l, -1, 1);
    x = lua_tonumber(l, -1);
    lua_pop(l, 1);
    lua_rawgeti(l, -1, 2);
    y = lua_tonumber(l, -1);
    lua_pop(l, 1);
    lua_rawgeti(l, -1, 3);
    if (lua_tonumber(l, -1) < 0) {
      g_warning("tetris_lua_get_fieldspec: Invalid negative cell type");
      return res;
    }
    cell = lua_tonumber(l, -1);
    lua_pop(l, 1);
    res = g_slist_prepend(res,
                          (gpointer) tetris_cell_info_new(x, y, cell));
    lua_pop(l, 1); /* pop the cell */
  }
  CHECK_STACK_END(l, 0);
  return res;
}

int l_players_all(lua_State *l)
{
  GSList *elem;
  TetrisPlayer *player;
  int i = 1;
  CHECK_STACK_START(l);

  lua_newtable(l);
  for (elem = tetris_player_all(); elem != NULL; elem = elem->next) {
    player = elem->data;
    lua_pushnumber(l, tetris_player_get_id(player));
    lua_rawseti(l, -2, i);
    i++;
  }
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_add(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_new(id);
  tetris_player_add(player);
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_nick(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    lua_pushstring(l, tetris_player_get_nick(player));
  } else {
    g_warning("tetris.players.get_nick: Player %d does not exists", id);
    lua_pushstring(l, "");
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_nick(lua_State *l)
{
  int id;
  char *nick;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  luaL_checktype(l, 2, LUA_TSTRING);
  nick = g_strdup(lua_tostring(l, 2));

  player = tetris_player_find(id);
  if (player != NULL) {
    tetris_player_set_nick(player, nick);
  } else {
    g_warning("tetris.players.set_nick: Player %d does not exists", id);
  }

  g_free(nick);
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_is_admin(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  if (player != NULL) {
    lua_pushboolean(l, tetris_player_is_admin(player));
  } else {
    g_warning("tetris.players.is_admin: Player %d does not exists", id);
    lua_pushboolean(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_admin(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  luaL_checktype(l, 2, LUA_TBOOLEAN);

  if (player != NULL) {
    tetris_player_set_admin(player, lua_toboolean(l, 2));
  } else {
    g_warning("tetris.players.set_admin: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_is_playing(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  if (player != NULL) {
    lua_pushboolean(l, tetris_player_is_playing(player));
  } else {
    g_warning("tetris.players.is_playing: Player %d does not exists", id);
    lua_pushboolean(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_playing(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  luaL_checktype(l, 2, LUA_TBOOLEAN);
  if (player != NULL) {
    tetris_player_set_playing(player, lua_toboolean(l, 2));
  } else {
    g_warning("tetris.players.set_playing: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    piece = tetris_player_get_piece(player);
  } else {
    g_warning("tetris.players.get_piece: Player %d does not exists", id);
    piece = NULL;
  }

  /* return the piece in a table */
  tetris_lua_push_fieldspec(l, piece);
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TTABLE);
  piece = tetris_lua_get_fieldspec(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_set_piece(player, piece);
  } else {
    g_warning("tetris.players.set_piece: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_piece_position(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  int *position;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  if (player != NULL) {
    position = tetris_player_get_piece_position(player);

    /* return the position as { x, y } */
    lua_newtable(l);
    lua_pushnumber(l, position[0]);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, position[1]);
    lua_rawseti(l, -2, 2);
  } else {
    g_warning("tetris.players.get_piece_position: Player %d does not exists", id);
    /* returns {0, 0} */
    lua_newtable(l);
    lua_pushnumber(l, 0);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, 0);
    lua_rawseti(l, -2, 2);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_piece_position(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  int position[2];
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  /* read the position parameter */
  luaL_checktype(l, 2, LUA_TTABLE);
  lua_rawgeti(l, 2, 1);
  position[0] = lua_tonumber(l, -1);
  lua_pop(l, 1);
  lua_rawgeti(l, 2, 2);
  position[1] = lua_tonumber(l, -1);
  lua_pop(l, 1);

  if (player != NULL) {
    /* and set the player's piece position */
    tetris_player_set_piece_position(player, position);
  } else {
    g_warning("tetris.players.set_piece_position: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_next_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    piece = tetris_player_get_next_piece(player);
  } else {
    g_warning("tetris.players.get_next_piece: Player %d does not exists", id);
    piece = NULL;
  }

  tetris_lua_push_fieldspec(l, piece);
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_next_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TTABLE);
  piece = tetris_lua_get_fieldspec(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_set_next_piece(player, piece);
  } else {
    g_warning("tetris.players.set_next_piece: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_kept_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    piece = tetris_player_get_kept_piece(player);
  } else {
    g_warning("tetris.players.get_kept_piece: Player %d does not exists", id);
    piece = NULL;
  }

  tetris_lua_push_fieldspec(l, piece);
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_set_kept_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TTABLE);
  piece = tetris_lua_get_fieldspec(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_set_kept_piece(player, piece);
  } else {
    g_warning("tetris.players.set_kept_piece: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_add_bonus(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  TetrisCell bonus;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  bonus = (TetrisCell) lua_tonumber(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_add_bonus(player, bonus);
  } else {
    g_warning("tetris.players.add_bonus: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_remove_bonus(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  TetrisCell bonus;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  bonus = (TetrisCell) lua_tonumber(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_remove_bonus(player, bonus);
  } else {
    g_warning("tetris.players.remove_bonus: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_has_bonus(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  TetrisCell bonus;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  bonus = (TetrisCell) lua_tonumber(l, 2);

  player = tetris_player_find(id);

  if (player != NULL) {
    lua_pushboolean(l, tetris_player_has_bonus(player, bonus));
  } else {
    g_warning("tetris.players.has_bonus: Player %d does not exists", id);
    lua_pushboolean(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1; /* return a boolean */
}

int l_players_first_bonus(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    lua_pushnumber(l, (int) tetris_player_get_first_bonus(player));
  } else {
    g_warning("tetris.players.first_bonus: Player %d does not exists", id);
    lua_pushnumber(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1; /* return a number */
}

int l_players_drop_bonus(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_drop_bonus(player);
  } else {
    g_warning("tetris.players.drop_bonus: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_add_points(lua_State *l)
{
  int id, points;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  /* player's id */
  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  /* points to add */
  luaL_checktype(l, 2, LUA_TNUMBER);
  points = lua_tonumber(l, 2);

  if (player != NULL) {
    /* add the points */
    tetris_player_add_points(player, points);
  } else {
    g_warning("tetris.players.add_points: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_reset_points(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  /* player's id */
  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  if (player != NULL) {
    /* reset the points */
    tetris_player_reset_points(player);
  } else {
    g_warning("tetris.players.reset_points: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_get_points(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  /* player's id */
  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  if (player != NULL) {
    /* return the points of the player */
    lua_pushnumber(l, tetris_player_get_points(player));
  } else {
    g_warning("tetris.players.get_points: Player %d does not exists", id);
    lua_pushnumber(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_players_remove(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);

  if (player != NULL) {
    tetris_player_remove(player);
  } else {
    g_warning("tetris.players.remove: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_nick_valid(lua_State *l)
{
  char *nick;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TSTRING);
  nick = g_strdup(lua_tostring(l, 1));

  lua_pushboolean(l, tetris_nick_is_valid(nick));
  g_free(nick);

  CHECK_STACK_END(l, 1);
  return 1; /* return a boolean */
}

int l_players_nick_available(lua_State *l)
{
  char *nick;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TSTRING);
  nick = g_strdup(lua_tostring(l, 1));

  lua_pushboolean(l, tetris_nick_is_available(nick));
  g_free(nick);

  CHECK_STACK_END(l, 1);
  return 1; /* return a boolean */
}

int l_players_exists(lua_State *l)
{
  int id;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  lua_pushboolean(l, tetris_player_find(id) != NULL);

  CHECK_STACK_END(l, 1);
  return 1; /* return a boolean */
}

int l_players_number(lua_State *l)
{
  int n;
  CHECK_STACK_START(l);

  n = g_slist_length(tetris_player_all(l));
  lua_pushnumber(l, n);

  CHECK_STACK_END(l, 1);
  return 1; /* return a number */
}

int l_matrix_set_cell(lua_State *l)
{
  int id, x, y;
  TetrisCell cell;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  x = lua_tonumber(l, 2);

  luaL_checktype(l, 3, LUA_TNUMBER);
  y = lua_tonumber(l, 3);

  luaL_checktype(l, 4, LUA_TNUMBER);
  if (lua_tonumber(l, 4) < 0) {
    g_warning("tetris.matrix.set_cell: Invalid negative cell type");
    return 0;
  }
  cell = (TetrisCell) lua_tonumber(l, 4);

  player = tetris_player_find(id);
  if (player != NULL) {
    tetris_matrix_set_cell(tetris_player_get_matrix(player),
                           x, y, cell);
  } else {
    g_warning("tetris.matrix.set_cell: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_matrix_get_cell(lua_State *l)
{
  int id, x, y;
  TetrisCell cell;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  x = lua_tonumber(l, 2);

  luaL_checktype(l, 3, LUA_TNUMBER);
  y = lua_tonumber(l, 3);

  player = tetris_player_find(id);
  if (player != NULL) {
    cell = tetris_matrix_get_cell(tetris_player_get_matrix(player),
                                  x, y);
  } else {
    g_warning("tetris.matrix.get_cell: Player %d does not exists", id);
    cell = (TetrisCell) 0;
  }
  lua_pushnumber(l, (int) cell);

  CHECK_STACK_END(l, 1);
  return 1; /* the cell */
}

int l_matrix_get_uncommited_cell(lua_State *l)
{
  int id, x, y;
  TetrisCell cell;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  x = lua_tonumber(l, 2);

  luaL_checktype(l, 3, LUA_TNUMBER);
  y = lua_tonumber(l, 3);

  player = tetris_player_find(id);
  if (player != NULL) {
    cell = tetris_matrix_get_uncommited_cell(tetris_player_get_matrix(player),
                                             x, y);
  } else {
    g_warning("tetris.matrix.get_uncommited_cell: Player %d does not exists", id);
    cell = (TetrisCell) 0;
  }
  lua_pushnumber(l, (int) cell);

  CHECK_STACK_END(l, 1);
  return 1; /* the cell */
}

int l_matrix_get_width(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  TetrisMatrix *matrix;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    matrix = tetris_player_get_matrix(player);
    lua_pushnumber(l, tetris_matrix_get_width(matrix));
  } else {
    g_warning("tetris.matrix.get_width: Player %d does not exists", id);
    lua_pushnumber(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_matrix_get_height(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  TetrisMatrix *matrix;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    matrix = tetris_player_get_matrix(player);
    lua_pushnumber(l, tetris_matrix_get_height(matrix));
  } else {
    g_warning("tetris.matrix.get_height: Player %d does not exists", id);
    lua_pushnumber(l, 0);
  }

  CHECK_STACK_END(l, 1);
  return 1;
}

int l_matrix_get_cells(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *cells;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    cells = tetris_matrix_get_cells(tetris_player_get_matrix(player));

    /* return a table containing all the filled cells */
    tetris_lua_push_fieldspec(l, cells);

    /* free the list of cells */
    g_slist_free_full(cells,
                      (GDestroyNotify) tetris_cell_info_free);
  } else {
    g_warning("tetris.matrix.get_cells: Player %d does not exists", id);
    cells = NULL;
    tetris_lua_push_fieldspec(l, cells);
  }

  CHECK_STACK_END(l, 1);
  return 1; /* return a table */
}

int l_matrix_diff(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *diff;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    diff = tetris_matrix_diff(tetris_player_get_matrix(player));

    /* return a new table which contains the infos */
    tetris_lua_push_fieldspec(l, diff);

    /* free the changes list */
    g_slist_free_full(diff,
                      (GDestroyNotify) tetris_cell_info_free);
  } else {
    g_warning("tetris.matrix.diff: Player %d does not exists", id);
    diff = NULL;
    tetris_lua_push_fieldspec(l, diff);
  }
  
  CHECK_STACK_END(l, 1);
  return 1; /* return a table */
}

int l_matrix_commit(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  if (player != NULL) {
    tetris_matrix_commit(tetris_player_get_matrix(player));
  } else {
    g_warning("tetris.matrix.commit: Player %d does not exists", id);
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_game_start(lua_State *l)
{
  CHECK_STACK_START(l);
  tetris_game_start();
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_game_stop(lua_State *l)
{
  CHECK_STACK_START(l);
  tetris_game_stop();
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_game_reset(lua_State *l)
{
  CHECK_STACK_START(l);
  tetris_game_reset();
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_game_is_started(lua_State *l)
{
  CHECK_STACK_START(l);
  lua_pushboolean(l, tetris_game_is_started());
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_game_pause(lua_State *l)
{
  CHECK_STACK_START(l);
  tetris_game_pause();
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_game_is_paused(lua_State *l)
{
  CHECK_STACK_START(l);
  lua_pushboolean(l, tetris_game_is_paused());
  CHECK_STACK_END(l, 1);
  return 1;
}
