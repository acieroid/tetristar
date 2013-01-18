#include "plugins.h"

static lua_State *lua_state;
static MainWindow *main_window;

static int l_send(lua_State *l);
static int l_chat_add_text(lua_State *l);
static int l_context_add_player(lua_State *l);
static int l_context_remove_player(lua_State *l);
static int l_context_field_changed(lua_State *l);
static int l_context_next_piece_changed(lua_State *l);
static int l_context_set_shadow(lua_State *l);
static int l_display_error(lua_State *l);
static int l_disconnect(lua_State *l);

static PluginFunction l_functions[] = {
  { "send", l_send },
  { "chat_add_text", l_chat_add_text },
  { "context_add_player", l_context_add_player },
  { "context_remove_player", l_context_remove_player },
  { "context_field_changed", l_context_field_changed },
  { "context_next_piece_changed", l_context_next_piece_changed },
  { "context_set_shadow", l_context_set_shadow },
  { "display_error", l_display_error },
  { "disconnect", l_disconnect },
  { NULL, NULL }
};

void plugins_init(lua_State *l, MainWindow *mw)
{
  lua_state = l;
  main_window = mw;
  tetris_plugin_add_category("client");
  tetris_plugin_add_functions("client", l_functions);
}

void plugins_deinit()
{
}

int l_send(lua_State *l)
{
  gchar *str;

  luaL_checktype(l, 1, LUA_TSTRING);
  str = g_strdup(lua_tostring(l, 1));

  network_send(NETWORK(main_window->network), str);
  g_free(str);
  return 0;
}

int l_chat_add_text(lua_State *l)
{
  gchar *str;

  luaL_checktype(l, 1, LUA_TSTRING);
  str = g_strdup(lua_tostring(l, 1));

  chat_add_text(CHAT(main_window->chat), str);
  g_free(str);
  return 0;
}
                
int l_context_add_player(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  context_add_player(CONTEXT(main_window->context), player);
  return 0;
}

int l_context_remove_player(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  context_remove_player(CONTEXT(main_window->context), player);
  return 0;
}

int l_context_field_changed(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  context_field_changed(CONTEXT(main_window->context), player);
  return 0;
}

int l_context_next_piece_changed(lua_State *l)
{
  int id;
  TetrisPlayer *player;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  context_next_piece_changed(CONTEXT(main_window->context), player);
  return 0;
}

int l_context_set_shadow(lua_State *l)
{
  int id;
  TetrisPlayer *player;
  GSList *shadow;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  player = tetris_player_find(id);

  luaL_checktype(l, 2, LUA_TTABLE);
  shadow = tetris_lua_get_fieldspec(l, 2);

  context_set_shadow(CONTEXT(main_window->context), player, shadow);
  return 0;
}

int l_display_error(lua_State *l)
{
  const gchar *message;

  luaL_checktype(l, 1, LUA_TSTRING);
  message = lua_tostring(l, 1);

  display_error(message);
  return 0;
}

int l_disconnect(lua_State *l)
{
  network_send(main_window->network, "BYE");
  return 0;
}
