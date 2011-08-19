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

#include "player.h"
#include "plugin.h"

/**
 * Create the lua functions
 */
void tetris_lua_functions_setup(void);

#endif /* LIBTETRIS_LUA_FUNCTIONS_H */
