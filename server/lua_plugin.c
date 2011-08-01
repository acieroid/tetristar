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
