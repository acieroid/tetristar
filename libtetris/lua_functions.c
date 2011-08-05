#include "lua_functions.h"

static const char *tables_to_create[] = { "players" };

static const struct {
  char *table;
  char *name;
  lua_CFunction fun;
} functions_to_export[] = {
  { "players", "add", l_players_add },
  { "players", "get_nick", l_players_get_nick },
  { "players", "set_nick", l_players_set_nick },
  { "players", "remove", l_players_remove },
  { "players", "nick_available", l_players_nick_available },
  { NULL, NULL, NULL }
};

void tetris_setup_lua(lua_State *l)
{
  int i;
  lua_newtable(l);

  /* create the subtables */
  for (i = 0; tables_to_create[i] != NULL; i++) {
    lua_newtable(l);
    lua_setfield(l, -2, tables_to_create[i]);
  }

  /* export the functions */
  for (i = 0; functions_to_export[i].name != NULL; i++) {
    lua_pushstring(l, functions_to_export[i].table);
    lua_gettable(l, -2); /* now we have tetris.table on top */
    lua_pushcfunction(l, functions_to_export[i].fun);
    /* and we set tetris.table.function */
    lua_setfield(l, -2, functions_to_export[i].name);
    lua_pop(l, 1);
  }

  lua_setglobal(l, "tetris");
}

int l_players_add(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_new(id);
  tetris_player_add(player);
  return 0;
}

int l_players_get_nick(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  lua_pushstring(l, tetris_player_get_nick(player));
  return 1;
}

int l_players_set_nick(lua_State *l)
{
  int id;
  char *nick;
  TetrisPlayer *player;
  
  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  luaL_checktype(l, 2, LUA_TSTRING);
  nick = strdup(lua_tostring(l, 2));

  player = tetris_player_find(id);
  tetris_player_set_nick(player, nick);
  return 0;
}

int l_players_remove(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  tetris_player_remove(player);
  return 0;
}

int l_players_nick_available(lua_State *l)
{
  char *nick;
  luaL_checktype(l, 1, LUA_TSTRING);
  nick = strdup(lua_tostring(l, 1));

  lua_pushboolean(l, tetris_nick_is_available(nick));
  free(nick);
  return 1; /* return a boolean */
}
