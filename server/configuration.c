#include "configuration.h"

void config_init(const char *file)
{
  assert(global_state != NULL);

  global_state->lua_state = lua_open();
  luaL_openlibs(LUA_STATE);

  DBG("Reading configuration file: %s", file);
  if (luaL_loadfile(LUA_STATE, file) != 0 ||
      lua_pcall(LUA_STATE, 0, 0, 0) != 0)
    WARN("Unable to load or run the configuration file: %s",
         lua_tostring(LUA_STATE, -1));
}

int config_get_int(const char *name, int def)
{
  int res = def;
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    /* uninitialized value, return the default value instead */
    break;
  case LUA_TNUMBER:
    res = (int) lua_tonumber(LUA_STATE, -1);
    break;
  default:
    WARN("%s should be a number, using default value (%d)", name, def);
    break;
  }

  lua_pop(LUA_STATE, 2);
  return res;
}

const char *config_get_string(const char *name, const char *def)
{
  char *res = strdup(def);
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    break;
  case LUA_TSTRING:
    free(res);
    res = strdup(lua_tostring(LUA_STATE, -1));
    break;
  default:
    WARN("%s should be a string, using default value (%s)", name, def);
    break;
  }

  lua_pop(LUA_STATE, 2);
  return (const char *) res;
}

GSList *config_get_list(const char *name, GSList *def)
{
  int i, size;
  GSList *res = def;
  lua_getglobal(LUA_STATE, "config");
  lua_getfield(LUA_STATE, -1, name);

  switch (lua_type(LUA_STATE, -1)) {
  case LUA_TNIL:
    break;
  case LUA_TTABLE:
    res = NULL;
    size = luaL_getn(LUA_STATE, -1);
    for (i = 1; i <= size; i++) {
      lua_rawgeti(LUA_STATE, -1, i);
      if (lua_isstring(LUA_STATE, -1))
        res = g_slist_prepend(res, (void *) strdup(lua_tostring(LUA_STATE, -1)));
      else
        WARN("The element %d of %s should be a string, ignoring it", i, name);
      lua_pop(LUA_STATE, 1);
    }
    res = g_slist_reverse(res);
    break;
  default:
    WARN("%s should be an array (a table) of strings, using default value", name);
    break;
  }

  lua_pop(LUA_STATE, 2);
  return res;
}

void config_deinit()
{
  lua_close(LUA_STATE);
}

