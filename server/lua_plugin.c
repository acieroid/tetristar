#include "lua_plugin.h"

void lua_plugin_load(const char *file, void *data)
{
  if (luaL_loadfile(LUA_STATE, file) ||
      lua_pcall(LUA_STATE, 0, 0, 0))
    WARN("Cannot load lua plugin: %s",
         lua_tostring(LUA_STATE, -1));
  DBG("Lua plugin %s loaded", file);
}

void lua_plugin_free(LuaPlugin *plugin, void *data)
{
  assert(plugin != NULL);
  free(plugin);
}

void lua_plugin_register(PluginType type, char *recv_command, LuaFunction function)
{
  LuaPlugin *plugin = malloc(sizeof(*plugin));
  assert(plugin != NULL);

  plugin->type = type;
  plugin->recv_command = recv_command;
  plugin->function = function;

  PLUGINS = g_slist_prepend(PLUGINS, (void *) plugin);
  DBG("Plugin of type %d registered for action %s", type, recv_command);

}

int l_register(lua_State *l)
{
  int type;
  char *type_descr, *recv_command;
  LuaFunction function;

  if (!lua_isstring(LUA_STATE, 1)) {
    WARN("First argument to register should be a string");
    return 0;
  }
    
  type_descr = strdup(lua_tostring(LUA_STATE, 1));
  recv_command = NULL;

  if (strcmp(type_descr, "CONNECT") == 0)
    type = PLUGIN_CONNECT;
  else if (strcmp(type_descr, "DISCONNECT") == 0)
    type = PLUGIN_DISCONNECT;
  else if (strcmp(type_descr, "RECV") == 0)
    type = PLUGIN_RECV;
  else {
    WARN("Unknown plugin type: %s", type_descr);
    return 0;
  }

  if (!lua_isfunction(LUA_STATE, 2)) {
    WARN("Second argument to register should be a function");
    return 0;
  }
  function = lua_topointer(LUA_STATE, 2);

  if (type == PLUGIN_RECV) {
    if (!lua_isstring(LUA_STATE, 3)) {
      WARN("Third argument to register should be a string");
      return 0;
    }
    recv_command = strdup(lua_tostring(LUA_STATE, 3));
  }

  lua_plugin_register(type, recv_command, function);
  return 0;
}
  
