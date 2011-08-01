#ifndef LUA_PLUGIN_H
#define LUA_PLUGIN_H

#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "global_state.h"
#include "util.h"

typedef struct LuaPlugin {
  int type;
} LuaPlugin;

void lua_plugin_load(const char *file, void *data);
void lua_plugin_free(LuaPlugin *plugin, void *data);

#endif /* LUA_PLUGIN_H */
