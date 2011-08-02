#include "configuration.h"

void config_init(const char *file)
{
  assert(global_state != NULL);

  global_state->lua_state = lua_open();
  luaL_openlibs(LUA_STATE);

  DBG("Reading configuration file: %s", file);
  if (luaL_loadfile(LUA_STATE, file) ||
      lua_pcall(LUA_STATE, 0, 0, 0))
    WARN("Unable to load or run the configuration file: %s",
         lua_tostring(LUA_STATE, -1));
}

int config_get_int(const char *name, int def)
{
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    /* uninitialized value, return the default value instead */
    return def;
  case LUA_TNUMBER:
    return (int) lua_tonumber(LUA_STATE, -1);
  default:
    WARN("%s should be a number, using default value (%d)", name, def);
    return def;
  }
}

const char *config_get_string(const char *name, const char *def)
{
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    return def;
  case LUA_TSTRING:
    return lua_tostring(LUA_STATE, -1);
  default:
    WARN("%s should be a string, using default value (%s)", name, def);
    return def;
  }
}

GSList *config_get_list(const char *name, GSList *def)
{
  int i, size;
  GSList *list = NULL;
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    return def;
  case LUA_TTABLE:
    size = luaL_getn(LUA_STATE, -1);
    for (i = 1; i <= size; i++) {
      lua_rawgeti(LUA_STATE, -1, i);
      if (lua_isstring(LUA_STATE, -1))
        list = g_slist_prepend(list, (void *) lua_tostring(LUA_STATE, -1));
      else
        WARN("The element %d of %s should be a string, ignoring it", i, name);
      lua_pop(LUA_STATE, 1);
    }
    return g_slist_reverse(list);
  default:
    WARN("%s should be an array (a table) of strings, using default value", name);
    return def;
  }
}

void config_deinit()
{
  lua_close(LUA_STATE);
}

