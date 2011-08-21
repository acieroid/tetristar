#include "plugins.h"

static int l_send(lua_State *l);
static int l_send_to_all(lua_State *l);
static int l_get_password(lua_State *l);

static PluginFunction l_functions[] = {
  { "send", l_send },
  { "send_to_all", l_send_to_all },
  { "get_password", l_get_password },
  { NULL, NULL },
};
  
void plugins_init()
{
  GSList *plugins_to_load;

  /* setup functions */
  tetris_plugin_add_category("server");
  tetris_plugin_add_functions("server", l_functions);

  /* load plugins */
  plugins_to_load = config_get_list("plugins", NULL);;
  g_slist_foreach(plugins_to_load,
                  (GFunc) tetris_plugin_file_load, NULL);
  g_slist_free_full(plugins_to_load, (GDestroyNotify) g_free);
}

void plugins_deinit()
{
}

int l_send(lua_State *l)
{
  int id;
  gchar *str = NULL;
  luaL_checktype(l, 1, LUA_TNUMBER);
  luaL_checktype(l, 2, LUA_TSTRING);

  id = lua_tonumber(l, 1);
  str = g_strdup(lua_tostring(l, 2));

  network_send(network_find_client(id), str);
  g_free(str);
  return 0;
}

int l_send_to_all(lua_State *l)
{
  gchar *str = NULL;
  luaL_checktype(l, 1, LUA_TSTRING);
  str = g_strdup(lua_tostring(l, 1));
  network_send_to_all(str);
  g_free(str);
  return 0;
}

int l_get_password(lua_State *l)
{
  char *password = (char *) config_get_string("password", "foo");
  lua_pushstring(l, password);
  free(password);
  return 1;
}
