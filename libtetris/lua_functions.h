/**
 * @file lua_functions.h
 * @brief Implements all the lua functions used to manipulate the game
 * state
 */
#ifndef LIBTETRIS_LUA_FUNCTIONS_H
#define LIBTETRIS_LUA_FUNCTIONS_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "plugin.h"
#include "player.h"
#include "game.h"

/**
 * Create the lua functions
 */
void tetris_lua_functions_setup(void);

/**
 * Push a field spec on the stack
 */
void tetris_lua_push_fieldspec(lua_State *l, GSList *spec);

/**
 * Get a field spec from the stack at the given index and return it
 */
GSList *tetris_lua_get_fieldspec(lua_State *l, int index);

#endif /* LIBTETRIS_LUA_FUNCTIONS_H */
