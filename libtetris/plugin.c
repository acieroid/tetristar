#include "plugin.h"

/* Timer multiplier (we want plugin timeouts in microseconds) */
#define TIMER_MULT 1000000

static lua_State *lua_state = NULL;
static GSList *plugins = NULL;
static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
static int waiting_time = 100000;
static GTimer *timer = NULL;

static void tetris_plugin_timeout_loop(gpointer data);

static int l_register(lua_State *l);
static int l_reset_timer(lua_State *l);
static int l_change_timeout(lua_State *l);

static PluginFunction plugin_functions[] = {
  { "register", l_register },
  { "reset_timer", l_reset_timer },
  { "change_timeout", l_change_timeout }
};

void tetris_plugin_init(lua_State *l)
{
  CHECK_STACK_START(l);
  lua_state = l;

  /* tetris = {} */
  lua_newtable(l);
  lua_setglobal(lua_state, "tetris");

  tetris_plugin_add_category("plugin");
  tetris_plugin_add_functions("plugin", plugin_functions);

  timer = g_timer_new();

  g_thread_new("libtetris",
               (GThreadFunc) tetris_plugin_timeout_loop,
               NULL);

  CHECK_STACK_END(l, 0);
}

void tetris_plugin_deinit()
{
  lua_state = NULL;
  tetris_plugin_unload_all();
  g_timer_destroy(timer);
}

void tetris_plugin_unload_all()
{
  g_slist_free_full(plugins, (GDestroyNotify) tetris_plugin_free);
  plugins = NULL;
}

Plugin *tetris_plugin_new(PluginType type,
                          const gchar *command,
                          LuaFunction fun,
                          int timeout)
{
  Plugin *plugin = g_malloc(sizeof(*plugin));
  plugin->type = type;
  plugin->command = g_strdup(command);
  plugin->function = fun;
  plugin->timeout = timeout;
  plugin->last_call = 0;
  plugin->next_call = 0;
  return plugin;
}

void tetris_plugin_free(Plugin *plugin)
{
  if (plugin->command != NULL)
    g_free(plugin->command);
  g_free(plugin);
}

Plugin *tetris_plugin_find(LuaFunction fun)
{
  GSList *elem;
  Plugin *plugin;
  CHECK_STACK_START(lua_state);

  for (elem = plugins; elem != NULL; elem = elem->next) {
    plugin = elem->data;
    /* push the functions to compare on the stack */
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, fun);
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, plugin->function);
    /* compare the functions */
    if (plugin->type == PLUGIN_TIMEOUT && lua_compare(lua_state, 1, 2, LUA_OPEQ)) {
      lua_pop(lua_state, 2);
      CHECK_STACK_END(lua_state, 0);
      return plugin;
    }
    /* pop compared functions from the stack */
    lua_pop(lua_state, 2);
  }

  CHECK_STACK_END(lua_state, 0);
  /* No plugin found */
  return NULL;
}

void tetris_plugin_add_category(const gchar *category)
{
  CHECK_STACK_START(lua_state);
  /* tetris.category = {} */
  lua_getglobal(lua_state, "tetris");
  lua_newtable(lua_state);
  lua_setfield(lua_state, -2, category);
  lua_pop(lua_state, 1);
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_add_function(const gchar *category,
                                PluginFunction fun)
{
  CHECK_STACK_START(lua_state);
  /* tetris.category.name = fun */
  lua_getglobal(lua_state, "tetris");
  lua_getfield(lua_state, -1, category);
  if (lua_type(lua_state, -1) != LUA_TTABLE) {
    g_warning("Category doesn't exists: %s", category);
    return;
  }

  lua_pushcfunction(lua_state, fun.fun);
  lua_setfield(lua_state, -2, fun.name);
  lua_pop(lua_state, 2);
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_add_functions(const gchar *category,
                                 PluginFunction funs[])
{
  CHECK_STACK_START(lua_state);
  int i;
  for (i = 0; funs[i].name != NULL; i++)
    tetris_plugin_add_function(category, funs[i]);
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_file_load(const gchar *file)
{
  CHECK_STACK_START(lua_state);
  if (luaL_loadfile(lua_state, file) != 0 ||
      lua_pcall(lua_state, 0, 0, 0) != 0) {
    g_warning("Error loading a plugin file: %s",
              lua_tostring(lua_state, -1));
    lua_pop(lua_state, 1);
  }
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_register(PluginType type,
                            const gchar *command,
                            LuaFunction function,
                            int timeout)
{
  CHECK_STACK_START(lua_state);
  Plugin *plugin = tetris_plugin_new(type, command, function, timeout);
  if (type == PLUGIN_TIMEOUT)
    waiting_time = tetris_gcd(waiting_time, timeout);
  plugins = g_slist_prepend(plugins, (gpointer) plugin);
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_action(PluginType type,
                          int id,
                          const gchar *command,
                          const gchar *args)
{
  GSList *elem;
  Plugin *plugin;
  int n_args;
  CHECK_STACK_START(lua_state);

  for (elem = plugins; elem != NULL; elem = elem->next) {
    plugin = elem->data;
    if (plugin->type == type) {
      g_static_mutex_lock(&mutex);

      /* the function */
      lua_rawgeti(lua_state, LUA_REGISTRYINDEX, plugin->function);

      n_args = 1;
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
          g_static_mutex_unlock(&mutex);
          continue;
        }
      }
      /* Execute the plugin */
      if (lua_pcall(lua_state, n_args, 0, 0) != 0) {
        g_warning("Error when calling a plugin: %s",
                  lua_tostring(lua_state, -1));
        lua_pop(lua_state, 1);
      }
      g_static_mutex_unlock(&mutex);
    }
  }
  CHECK_STACK_END(lua_state, 0);
}

void tetris_plugin_timeout_loop(gpointer data)
{
  GSList *elem;
  Plugin *plugin;

  while(TRUE) {
    for (elem = plugins; elem != NULL; elem = elem->next) {
      plugin = elem->data;

      if (plugin->type == PLUGIN_TIMEOUT &&
          ((int) (TIMER_MULT*g_timer_elapsed(timer, NULL)) >
           plugin->next_call)) {
        g_static_mutex_lock(&mutex);

        /* the function */
        lua_rawgeti(lua_state, LUA_REGISTRYINDEX, plugin->function);
        /* pass the delta as argument */
        lua_pushnumber(lua_state,
                       (int) (TIMER_MULT*g_timer_elapsed(timer, NULL)) - plugin->last_call);
        /* Execute the plugin */
        if (lua_pcall(lua_state, 1, 0, 0) != 0) {
          g_warning("Error when calling a plugin: %s",
                    lua_tostring(lua_state, -1));
          lua_pop(lua_state, 1);
        }
        plugin->last_call = (int) (TIMER_MULT*g_timer_elapsed(timer, NULL));
        plugin->next_call = plugin->last_call + plugin->timeout;

        g_static_mutex_unlock(&mutex);
      }
    }
    tetris_usleep(waiting_time);
  }
}

int l_register(lua_State *l)
{
  int type, timeout;
  gchar *type_descr, *recv_command;
  LuaFunction function;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TSTRING);
    
  type_descr = g_strdup(lua_tostring(l, 1));
  recv_command = NULL;

  if (g_strcmp0(type_descr, "CONNECT") == 0)
    type = PLUGIN_CONNECT;
  else if (g_strcmp0(type_descr, "DISCONNECT") == 0)
    type = PLUGIN_DISCONNECT;
  else if (g_strcmp0(type_descr, "SHUTDOWN") == 0)
    type = PLUGIN_SHUTDOWN;
  else if (g_strcmp0(type_descr, "RECV") == 0)
    type = PLUGIN_RECV;
  else if (g_strcmp0(type_descr, "TIMEOUT") == 0)
    type = PLUGIN_TIMEOUT;
  else {
    g_warning("Unknown plugin type: %s", type_descr);
    return 0;
  }
  g_free(type_descr);

  luaL_checktype(l, 2, LUA_TFUNCTION);
  lua_pushvalue(l, 2);
  function = luaL_ref(l, LUA_REGISTRYINDEX);

  if (type == PLUGIN_RECV) {
    luaL_checktype(l, 3, LUA_TSTRING);
    recv_command = g_strdup(lua_tostring(l, 3));
  }
  else if (type == PLUGIN_TIMEOUT) {
    luaL_checktype(l, 3, LUA_TNUMBER);
    timeout = lua_tonumber(l, 3);
  }

  tetris_plugin_register(type, recv_command, function, timeout);
  if (recv_command != NULL)
    g_free(recv_command);
  CHECK_STACK_END(l, 0);
  return 0;
}

int l_reset_timer(lua_State *l)
{
  Plugin *plugin;
  LuaFunction function;
  CHECK_STACK_START(l);

  luaL_checktype(l, 1, LUA_TFUNCTION);
  function = luaL_ref(l, LUA_REGISTRYINDEX);

  plugin = tetris_plugin_find(function);
  if (plugin && plugin->type == PLUGIN_TIMEOUT) {
    /* reset the timer of this plugin */
    plugin->next_call = (int) (TIMER_MULT*g_timer_elapsed(timer, NULL)) +
      plugin->timeout;
  } else {
    g_warning("reset_timer argument should be a TIMEOUT plugin function");
  }

  CHECK_STACK_END(l, 0);
  return 0;
}

int l_change_timeout(lua_State *l)
{
  Plugin *plugin;
  LuaFunction function;
  int timeout;
  CHECK_STACK_START(l);

  /* the plugin's function */
  luaL_checktype(l, 1, LUA_TFUNCTION);
  lua_pushvalue(l, 1);
  function = luaL_ref(l, LUA_REGISTRYINDEX);

  /* the new timeout */
  luaL_checktype(l, 2, LUA_TNUMBER);
  timeout = lua_tonumber(l, 2);

  plugin = tetris_plugin_find(function);
  if (plugin && plugin->type == PLUGIN_TIMEOUT) {
    /* change the timeout of this plugin */
    plugin->timeout = timeout;
  } else {
    g_warning("change_timeout argument should be a TIMEOUT plugin function");
  }

  CHECK_STACK_END(l, 0);
  return 0;
}
