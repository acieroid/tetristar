#include "plugins.h"

void plugins_init()
{
  global_state->plugins = NULL;

  /* setup functions */
  lua_pushcfunction(LUA_STATE, l_register);
  lua_setglobal(LUA_STATE, "register");

  /* load plugins */
  g_slist_foreach(config_get_list("plugins", NULL),
                  (GFunc) lua_plugin_load, NULL);
}

void plugins_deinit()
{
  g_slist_foreach(PLUGINS, (GFunc) lua_plugin_free, NULL);
}
