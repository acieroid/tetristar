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
static int l_players_nick_available(lua_State *l);
static int l_players_exists(lua_State *l);
static int l_players_number(lua_State *l);

static int l_matrix_set_cell(lua_State *l);
static int l_matrix_get_cell(lua_State *l);
static int l_matrix_get_uncommited_cell(lua_State *l);
static int l_matrix_get_width(lua_State *l);
static int l_matrix_get_height(lua_State *l);
static int l_matrix_diff(lua_State *l);
static int l_matrix_commit(lua_State *l);

static int l_game_start(lua_State *l);
static int l_game_stop(lua_State *l);
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
      { "remove", l_players_remove },
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
    if (lua_objlen(l, -1) != 3) {
      g_warning("Invalid cell");
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
      g_warning("Invalid negative cell type");
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
  lua_pushstring(l, tetris_player_get_nick(player));
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
  tetris_player_set_nick(player, nick);
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

  lua_pushboolean(l, tetris_player_is_admin(player));
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
  tetris_player_set_admin(player, lua_toboolean(l, 2));

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

  lua_pushboolean(l, tetris_player_is_playing(player));

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
  tetris_player_set_playing(player, lua_toboolean(l, 2));

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
  piece = tetris_player_get_piece(player);

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
  tetris_player_set_piece(player, piece);
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
  position = tetris_player_get_piece_position(player);

  /* return the position as { x, y } */
  lua_newtable(l);
  lua_pushnumber(l, position[0]);
  lua_rawseti(l, -2, 1);
  lua_pushnumber(l, position[1]);
  lua_rawseti(l, -2, 2);

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

  /* and set the player's piece position */
  tetris_player_set_piece_position(player, position);
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_players_remove(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  tetris_player_remove(player);
  CHECK_STACK_END(l, 0);
  return 0;
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
    g_warning("Invalid negative cell type");
    return 0;
  }
  cell = (TetrisCell) lua_tonumber(l, 4);

  player = tetris_player_find(id);
  tetris_matrix_set_cell(tetris_player_get_matrix(player),
                         x, y, cell);
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
  cell = tetris_matrix_get_cell(tetris_player_get_matrix(player),
                                x, y);
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
  cell = tetris_matrix_get_uncommited_cell(tetris_player_get_matrix(player),
                                           x, y);
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
  matrix = tetris_player_get_matrix(player);
  lua_pushnumber(l, tetris_matrix_get_width(matrix));

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
  matrix = tetris_player_get_matrix(player);
  lua_pushnumber(l, tetris_matrix_get_height(matrix));

  CHECK_STACK_END(l, 1);
  return 1;
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
  diff = tetris_matrix_diff(tetris_player_get_matrix(player));

  /* return a new table which contains the infos */
  tetris_lua_push_fieldspec(l, diff);
  CHECK_STACK_END(l, 1);
  return 1;
}

int l_matrix_commit(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);

  player = tetris_player_find(id);
  tetris_matrix_commit(tetris_player_get_matrix(player));
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
