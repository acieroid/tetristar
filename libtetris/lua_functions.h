#ifndef LIBTETRIS_LUA_FUNCTIONS_H
#define LIBTETRIS_LUA_FUNCTIONS_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "player.h"

void tetris_setup_lua(lua_State *l);

int l_players_all(lua_State *l);
int l_players_add(lua_State *l);
int l_players_get_nick(lua_State *l);
int l_players_set_nick(lua_State *l);
int l_players_remove(lua_State *l);
int l_players_nick_available(lua_State *l);
int l_players_exists(lua_State *l);

#endif /* LIBTETRIS_LUA_FUNCTIONS_H */
