#include "plugins.h"

static lua_State *lua_state = NULL;
static GSList *plugins = NULL;

static int l_register(lua_State *l);

static PluginFunction plugin_functions[] = {
  { "register", l_register }
};

void tetris_plugin_init(lua_State *l)
{
  lua_state = l;

  /* tetris = {} */
  lua_newtable(l);
  lua_setglobal(lua_state, "tetris");

  tetris_plugin_add_category("plugin");
  tetris_plugin_add_functions("plugin", plugin_functions);
}

void tetris_plugin_deinit()
{
  lua_state = NULL;
  g_slist_free_full(plugins, (GDestroyNotify) tetris_plugin_free);
}

Plugin *tetris_plugin_new(PluginType type,
                          const gchar *command,
                          LuaFunction fun)
{
  Plugin *plugin = g_malloc(sizeof(*plugin));
  plugin->type = type;
  plugin->command = g_strdup(command);
  plugin->function = function;
  return plugin;
}

void tetris_plugin_free(Plugin *plugin)
{
  if (plugin->command != NULL)
    g_free(plugin->command);
  g_free(plugin);
}

void tetris_plugin_add_category(const gchar *category)
{
  /* tetris.category = {} */
  lua_getglobal(lua_state, "tetris");
  lua_newtable(lua_state);
  lua_setfield(lua_state, -2, category);
  lua_pop(lua_state, 1);
}

void tetris_plugin_add_function(const gchar *category,
                                PluginFunction fun)
{
  /* tetris.category.name = fun */
  lua_getglobal(lua_state, "tetris");
  lua_getfield(lua_state, -1, category);
  if (lua_type(lua_state, -1) != LUA_TTABLE) {
    WARN("%s is not defined or is not a table", category);
    return;
  }

  lua_pushcfunction(lua_state, fun.fun);
  lua_setfield(lua_state, -2, fun.name);
  lua_pop(lua_state, 1);
}

void tetris_plugin_add_functions(const gchar *category,
                                 PluginFunction funs[])
{
  int i;
  for (i = 0; funs[i] != NULL; i++)
    tetris_plugin_add_function(category, funs[i]);
}

void tetris_plugin_file_load(const gchar *file)
{
  if (luaL_loadfile(lua_state, file) != 0 ||
      lua_pcall(lua_state, 0, 0, 0) != 0)
    WARN("Error loading a plugin file: %s",
         lua_tostring(lua_state, -1));
}

void tetris_plugin_register(PluginType type,
                            const gchar *command,
                            LuaFunction function)
{
  Plugin *plugin = tetris_plugin_new(type, command, function);
  plugins = g_slist_prepend(plugins, (gpointer) plugin);
}

void tetris_plugin_action(PluginType type,
                          const gchar *command,
                          const gchar *args)
{
  GSList *elem;
  Plugin *plugin;
  int n_args;

  for (elem = plugins; elem != NULL; elem = elem->next) {
    plugin = elem->data;
    if (plugin->type == type) {
      /* the function */
      lua_rawgeti(lua_state, LUA_REGISTRYINDEX, plugin->function);

      nargs = 1;
      /* first arg: the id of the client */
      lua_pushnumber(lua_state, id);

      if (type == PLUGIN_RECV) {
        if (g_strcmp0(plugin->command, command) == 0) {
          /* second arg: the command */
          lua_pushstring(lua_state, command);
          /* third arg: the actual arguments */
          lua_pushstring(lua_state, args);
          n_args = 3;
        }
        else {
          /* the command doesn't match, we restore the stack and skip
             this iteration */
          lua_pop(lua_state, 2);
          continue;
        }
      }
      /* Execute the plugin */
      if (lua_pcall(lua_state, n_args, 0, 0) != 0)
        WARN("Error when calling a plugin: %s",
             lua_tostring(lua_state, -1));
    }
  }
}
