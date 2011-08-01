#ifndef LUA_PLUGIN_H
#define LUA_PLUGIN_H

#include <assert.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "global_state.h"
#include "util.h"

typedef const void* LuaFunction;

typedef enum PluginType {
  PLUGIN_CONNECT,
  PLUGIN_DISCONNECT,
  PLUGIN_RECV
} PluginType;

typedef struct LuaPlugin {
  PluginType type;
  char *recv_command;
  LuaFunction function;
} LuaPlugin;

void lua_plugin_load(const char *file, void *data);
void lua_plugin_free(LuaPlugin *plugin, void *data);
void lua_plugin_register(PluginType type, char *recv_command, LuaFunction function);

int l_register(lua_State *L);

#endif /* LUA_PLUGIN_H */
