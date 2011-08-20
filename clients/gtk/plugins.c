#include "plugins.h"

static lua_State *lua_state;
static MainWindow *main_window;

static int l_send(lua_State *l);
static int l_chat_add_text(lua_State *l);

static PluginFunction l_functions[] = {
  { "send", l_send },
  { "chat_add_text", l_chat_add_text },
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
                
