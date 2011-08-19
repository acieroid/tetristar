#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glib.h>
#include <enet/enet.h>

typedef struct GlobalState {
  lua_State *lua_state;
  GSList *plugins;
  Network *network;
} GlobalState;

extern GlobalState *global_state;

#define LUA_STATE global_state->lua_state
#define PLUGINS global_state->plugins
#define NETWORK global_state->network

void global_state_init();
void global_state_deinit();

#endif /* GLOBAL_STATE_H */
