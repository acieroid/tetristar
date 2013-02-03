/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file plugins.h
 * @brief Implements plugins.
 */
#ifndef LIBTETRIS_PLUGINS_H
#define LIBTETRIS_PLUGINS_H

#include <glib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "util.h"

#ifdef DEBUG
#define CHECK_STACK_START(l)                    \
  int lua_stack_size = lua_gettop((l));
#define CHECK_STACK_END(l, n)                                           \
  if (lua_gettop((l)) != lua_stack_size + n)                            \
    g_debug("Lua stack size changed: in %s at %s:%d: %d -> %d",         \
            __FUNCTION__, __FILE__, __LINE__, lua_stack_size, lua_gettop((l)));

#else
#define CHECK_STACK_START(l) ;
#define CHECK_STACK_END(l, n) ;
#endif

typedef int LuaFunction;

/**
 * The type of a plugin describe when the plugin will be called
 */
typedef enum PluginType {
  PLUGIN_CONNECT,
  PLUGIN_DISCONNECT,
  PLUGIN_SHUTDOWN,
  PLUGIN_RECV,
  PLUGIN_TIMEOUT,
} PluginType;

/**
 * Plugins are simple lua functions that will be called on event
 * (connection, disconnection, shutdown, command received)
 */
typedef struct Plugin {
  PluginType type;
  gchar *command;
  LuaFunction function;
  int timeout, last_call, next_call;
} Plugin;

/**
 * A plugin function is a C function which is callable from a plugin
 */
typedef struct PluginFunction {
  gchar *name;
  lua_CFunction fun;
} PluginFunction;

/**
 * Initialize the plugins by setting up the needed functions (register
 * etc.)
 */
void tetris_plugin_init(lua_State *l);

/**
 * Deinitialize the plugins (ie. free everything)
 */
void tetris_plugin_deinit();

/**
 * Unload all the plugins
 */
void tetris_plugin_unload_all();

/**
 * Allocate and return a new plugin
 */
Plugin *tetris_plugin_new(PluginType type,
                          const gchar *command,
                          LuaFunction fun,
                          int timeout);

/**
 * Free a plugin created with tetris_plugin_new
 */
void tetris_plugin_free(Plugin *plugin);

/**
 * Find a plugin given a reference to its lua function. Returns NULL
 * if no plugin matched the function given as argument.
 */
Plugin *tetris_plugin_find(LuaFunction fun);

/**
 * Add a new category of plugin functions
 */
void tetris_plugin_add_category(const gchar *category);

/**
 * Add a new plugin function
 */
void tetris_plugin_add_function(const gchar *category,
                                PluginFunction fun);

/**
 * Add multiple new plugin functions in the same category
 * @param funs should be terminated by a { NULL, NULL } couple
 */
void tetris_plugin_add_functions(const gchar *category,
                                 PluginFunction funs[]);

/**
 * Evaluate a file, which will register itself plugins
 */
void tetris_plugin_file_load(const gchar *file);

/**
 * Register a plugin
 */
void tetris_plugin_register(PluginType type,
                            const gchar *command,
                            LuaFunction function,
                            int timeout);

/**
 * Call all the plugins that should be called on a certain action
 * @param id is the id of the client who sent that command (not
 * important if the program calling tetris_plugin_action is not a
 * server or when the action is PLUGIN_SHUTDOWN)
 */
void tetris_plugin_action(PluginType type,
                          int id,
                          const gchar *command,
                          const gchar *args);

#endif /* LIBTETRIS_PLUGIN_H */
