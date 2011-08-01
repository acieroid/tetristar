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

void plugins_on_action(PluginType type, int id, char *command, char *args)
{
  GSList *elem;
  LuaPlugin *plugin;
  int nargs;

  for (elem = PLUGINS; elem != NULL; elem = elem->next) {
    plugin = elem->data;

    nargs = 1;
    lua_pushnumber(LUA_STATE, id);
    if (plugin->type == type) {
      if (type == PLUGIN_RECV &&
          strcmp(plugin->recv_command, command) == 0) {
        lua_pushstring(LUA_STATE, command);
        lua_pushstring(LUA_STATE, args);
        nargs = 3;
      }
      lua_rawgeti(LUA_STATE, LUA_REGISTRYINDEX, plugin->function);
      printf("Function: %d\n", lua_isfunction(LUA_STATE, -1));
      if (lua_pcall(LUA_STATE, nargs, 0, 0) != 0)
        WARN("Error when calling a plugin action: %s",
             lua_tostring(LUA_STATE, -1));
    }
  }
}
