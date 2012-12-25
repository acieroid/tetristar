#include "configuration.h"

static lua_State *lua_state;

void config_init(lua_State *l, const char *file)
{
  lua_state = l;

  g_debug("Reading configuration file: %s", file);
  if (luaL_loadfile(lua_state, file) != 0 ||
      lua_pcall(lua_state, 0, 0, 0) != 0)
    g_warning("Unable to load or run the configuration file: %s",
              lua_tostring(lua_state, -1));
}

void config_deinit()
{
}

int config_get_int(const gchar *name, int def)
{
  int res = def;
  lua_getglobal(lua_state, "config");
  lua_getfield(lua_state, -1, name);

  switch (lua_type(lua_state, -1)) {
  case LUA_TNIL:
    /* uninitialized value, return the default value instead */
    break;
  case LUA_TNUMBER:
    res = (int) lua_tonumber(lua_state, -1);
    break;
  default:
    g_warning("%s should be a number, using default value (%d)",
              name, def);
    break;
  }

  lua_pop(lua_state, 2);
  return res;
}

const gchar *config_get_string(const gchar *name, const gchar *def)
{
  gchar *res = g_strdup(def);
  lua_getglobal(lua_state, "config");
  lua_getfield(lua_state, -1, name);

  switch (lua_type(lua_state, -1)) {
  case LUA_TNIL:
    break;
  case LUA_TSTRING:
    g_free(res);
    res = g_strdup(lua_tostring(lua_state, -1));
    break;
  default:
    g_warning("%s should be a string, using default value (%s)",
              name, def);
    break;
  }

  lua_pop(lua_state, 2);
  return (const gchar *) res;
}

GSList *config_get_list(const gchar *name, GSList *def)
{
  int i, size;
  GSList *res = def;
  lua_getglobal(lua_state, "config");
  lua_getfield(lua_state, -1, name);

  switch (lua_type(lua_state, -1)) {
  case LUA_TNIL:
    break;
  case LUA_TTABLE:
    res = NULL;
#if LUA_VERSION_NUM >= 502
    size = lua_rawlen(lua_state, -1);
#else
    size = lua_objlen(lua_state, -1);
#endif
    for (i = 1; i <= size; i++) {
      lua_rawgeti(lua_state, -1, i);
      if (lua_isstring(lua_state, -1))
        res = g_slist_prepend(res, (gpointer) g_strdup(lua_tostring(lua_state, -1)));
      else
        g_warning("The element %d of %s should be a string, ignoring it",
                  i, name);
      lua_pop(lua_state, 1);
    }
    res = g_slist_reverse(res);
    break;
  default:
    g_warning("%s should be an array (a table) of strings, using default value",
              name);
    break;
  }

  lua_pop(lua_state, 2);
  return res;
}
