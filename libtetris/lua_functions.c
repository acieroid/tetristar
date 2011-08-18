#include "lua_functions.h"

static int l_players_all(lua_State *l);
static int l_players_add(lua_State *l);
static int l_players_remove(lua_State *l);
static int l_players_get_nick(lua_State *l);
static int l_players_set_nick(lua_State *l);
static int l_players_nick_available(lua_State *l);
static int l_players_exists(lua_State *l);

static struct {
  gchar *category;
  /* we need to have a fixed size to compile */
  PluginFunction functions[128];
} l_functions[] = {
  {
    "player", {
      { "all", l_players_all },
      { "add", l_players_add },
      { "get_nick", l_players_get_nick },
      { "set_nick", l_players_set_nick },
      { "remove", l_players_remove },
      { "nick_available", l_players_nick_available },
      { "exists", l_players_exists },
      { NULL, NULL },
    }
  },
  { NULL, {} },
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

int l_players_all(lua_State *l)
{
  GSList *elem;
  TetrisPlayer *player;
  int i = 1;

  lua_newtable(l);
  for (elem = tetris_player_all(); elem != NULL; elem = elem->next) {
    player = elem->data;
    lua_pushnumber(l, tetris_player_get_id(player));
    lua_rawseti(l, -2, i);
    i++;
  }
  return 1;
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
  nick = g_strdup(lua_tostring(l, 2));

  player = tetris_player_find(id);
  tetris_player_set_nick(player, nick);
  g_free(nick);
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
  nick = g_strdup(lua_tostring(l, 1));

  lua_pushboolean(l, tetris_nick_is_available(nick));
  g_free(nick);
  return 1; /* return a boolean */
}

int l_players_exists(lua_State *l)
{
  int id;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  lua_pushboolean(l, tetris_player_find(id) != NULL);
  return 1; /* return a boolean */
}
