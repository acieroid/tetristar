#ifndef LUA_PLUGIN_H
#define LUA_PLUGIN_H

#include <assert.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "global_state.h"
#include "network.h"
#include "util.h"

typedef int LuaFunction;

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
void lua_plugin_free(LuaPlugin *plugin);
void lua_plugin_register(PluginType type, char *recv_command, LuaFunction function);
void lua_plugin_setup_functions();

int l_register(lua_State *l);
int l_send(lua_State *l);
int l_send_to_all(lua_State *l);
int l_get_password(lua_State *l);

static const struct {
  char *name;
  lua_CFunction fun;
} functions_to_export[] = {
  { "register", l_register },
  { "send", l_send },
  { "send_to_all", l_send_to_all },
  { "get_password", l_get_password },
  { NULL, NULL }
};

#endif /* LUA_PLUGIN_H */
