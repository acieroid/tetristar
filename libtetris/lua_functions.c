#include "lua_functions.h"

static int l_players_all(lua_State *l);
static int l_players_add(lua_State *l);
static int l_players_remove(lua_State *l);
static int l_players_get_nick(lua_State *l);
static int l_players_set_nick(lua_State *l);
static int l_players_get_piece(lua_State *l);
static int l_players_set_piece(lua_State *l);
static int l_players_nick_available(lua_State *l);
static int l_players_exists(lua_State *l);

static int l_matrix_set_cell(lua_State *l);
static int l_matrix_get_cell(lua_State *l);
static int l_matrix_diff(lua_State *l);
static int l_matrix_commit(lua_State *l);

static int l_game_start(lua_State *l);
static int l_game_stop(lua_State *l);
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
      { "get_piece", l_players_get_piece },
      { "set_piece", l_players_set_piece },
      { "remove", l_players_remove },
      { "nick_available", l_players_nick_available },
      { "exists", l_players_exists },
      { NULL, NULL }
    }
  },
  { "matrix", {
      { "set_cell", l_matrix_set_cell },
      { "get_cell", l_matrix_get_cell },
      { "diff", l_matrix_diff },
      { "commit", l_matrix_commit },
      { NULL, NULL }
    }
  },
  { "game", {
      { "start", l_game_start },
      { "stop", l_game_stop },
      { "is_started", l_game_is_started },
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
}

GSList *tetris_lua_get_fieldspec(lua_State *l, int index)
{
  GSList *res = NULL;
  int x, y;
  TetrisCell cell;

  /* iterate over the table at index */
  lua_pushnil(l);
  while (lua_next(l, index) != 0) {
    /* a table representing a cell is on the top of the stack */
    lua_rawgeti(l, -1, 1);
    x = lua_tonumber(l, -1);
    lua_rawgeti(l, -1, 2);
    y = lua_tonumber(l, -1);
    lua_rawgeti(l, -1, 3);
    if (lua_tonumber(l, -1) < 0) {
      g_warning("Invalid negative cell type");
      return res;
    }
    cell = lua_tonumber(l, -1);
    res = g_slist_prepend(res,
                          (gpointer) tetris_cell_info_new(x, y, cell));
  }
  return res;
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

int l_players_get_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  piece = tetris_player_get_piece(player);

  /* return the piece in a table */
  tetris_lua_push_fieldspec(l, piece);
  return 1;
}

int l_players_set_piece(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *piece;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TTABLE);
  piece = tetris_lua_get_fieldspec(l, 2);

  player = tetris_player_find(id);
  tetris_player_set_piece(player, piece);
 
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

int l_matrix_set_cell(lua_State *l)
{
  int id, x, y;
  TetrisCell cell;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  x = lua_tonumber(l, 2);

  luaL_checktype(l, 3, LUA_TNUMBER);
  y = lua_tonumber(l, 3);

  luaL_checktype(l, 4, LUA_TNUMBER);
  if (lua_tonumber(l, 4) < 0) {
    g_warning("Invalid negative cell type");
    return 0;
  }
  cell = (TetrisCell) lua_tonumber(l, 4);

  player = tetris_player_find(id);
  tetris_matrix_set_cell(tetris_player_get_matrix(player),
                         x, y, cell);
  return 0;
}

int l_matrix_get_cell(lua_State *l)
{
  int id, x, y;
  TetrisCell cell;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  luaL_checktype(l, 2, LUA_TNUMBER);
  x = lua_tonumber(l, 2);

  luaL_checktype(l, 3, LUA_TNUMBER);
  y = lua_tonumber(l, 3);

  player = tetris_player_find(id);
  cell = tetris_matrix_get_cell(tetris_player_get_matrix(player),
                                x, y);
  lua_pushnumber(l, (int) cell);
  return 1;
}

int l_matrix_diff(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *diff;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  diff = tetris_matrix_diff(tetris_player_get_matrix(player));

  /* return a new table which contains the infos */
  tetris_lua_push_fieldspec(l, diff);

  return 1;
}

int l_matrix_commit(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  tetris_matrix_commit(tetris_player_get_matrix(player));
  return 0;
}

int l_game_start(lua_State *l)
{
  tetris_game_start();
  return 0;
}

int l_game_stop(lua_State *l)
{
  tetris_game_stop();
  return 0;
}

int l_game_is_started(lua_State *l)
{
  gboolean started = tetris_game_is_started();
  lua_pushnumber(l, (int) started);
  return 1;
}
