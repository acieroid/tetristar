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
  if (plugin->recv_command != NULL)
    free(plugin->recv_command);
  luaL_unref(LUA_STATE, LUA_REGISTRYINDEX, plugin->function);
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
  if (type == PLUGIN_RECV)
    DBG("Plugin of type RECV registered for action %s", recv_command);
  else if (type == PLUGIN_CONNECT)
    DBG("Plugin of type CONNECT registered");
  else if (type == PLUGIN_DISCONNECT)
    DBG("Plugin of type DISCONNECT registered");
}

int l_register(lua_State *l)
{
  int type;
  char *type_descr, *recv_command;
  LuaFunction function;

  if (!lua_isstring(l, 1)) {
    WARN("First argument to server.register should be a string");
    return 0;
  }
    
  type_descr = strdup(lua_tostring(l, 1));
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

  if (!lua_isfunction(l, 2)) {
    WARN("Second argument to server.register should be a function");
    return 0;
  }
  lua_pushvalue(l, 2);
  function = luaL_ref(l, LUA_REGISTRYINDEX);

  if (type == PLUGIN_RECV) {
    if (!lua_isstring(l, 3)) {
      WARN("Third argument to server.register should be a string");
      return 0;
    }
    recv_command = strdup(lua_tostring(l, 3));
    assert(recv_command != NULL);
  }

  lua_plugin_register(type, recv_command, function);
  return 0;
}

int l_send(lua_State *l)
{
  int id;
  char *str = NULL;
  /* TODO: use luaL_checktype */
  if (!lua_isnumber(l, 1))
    WARN("First argument to server.send should be a number");
  if (!lua_isstring(l, 2))
    WARN("Second argument to server.send should be a string");

  id = lua_tonumber(l, 1);
  str = strdup(lua_tostring(l, 2));
  assert(str != NULL);

  network_send(network_find_client(id), str);
  free(str);
  return 0;
}

int l_send_to_all(lua_State *l)
{
  char *str = NULL;
  if (!lua_isstring(l, 1))
    WARN("First argument to server.send_to_all should be a number");
  str = strdup(lua_tostring(l, 1));
  network_send_to_all(str);
  free(str);
  return 0;
}

void lua_plugin_setup_functions()
{
  int i;
  /* embed functions in a table */
  lua_newtable(LUA_STATE);

  for (i = 0; functions_to_export[i].fun != NULL; i++) {
    /* set server.function_name */
    lua_pushcfunction(LUA_STATE, functions_to_export[i].fun);
    lua_setfield(LUA_STATE, -2, functions_to_export[i].name);
  }

  lua_setglobal(LUA_STATE, "server");
}
