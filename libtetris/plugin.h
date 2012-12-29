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
