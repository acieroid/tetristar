#include "configuration.h"

Config *config_init(const char *file)
{
  Config *config = malloc(sizeof(*config));
  assert(config != NULL);

  config->state = lua_open();
  luaL_openlibs(config->state);

  DBG("Reading configuration file: %s", file);
  if (luaL_loadfile(config->state, file) ||
      lua_pcall(config->state, 0, 0, 0))
    WARN("Unable to load or run the configuration file: %s",
         lua_tostring(config->state, -1));

  return config;
}

int config_get_int(Config *config, const char *name, int def)
{
  assert(config != NULL);
  lua_getglobal(config->state, name);

  switch (lua_type(config->state, -1)) {
  case LUA_TNIL:
    /* uninitialized value, return the default value instead */
    return def;
  case LUA_TNUMBER:
    return (int) lua_tonumber(config->state, -1);
  default:
    WARN("%s should be a number, using default value (%d)", name, def);
    return def;
  }
}

const char *config_get_string(Config *config, const char *name, const char *def)
{
  assert(config != NULL);
  lua_getglobal(config->state, name);

  switch (lua_type(config->state, -1)) {
  case LUA_TNIL:
    return def;
  case LUA_TSTRING:
    return lua_tostring(config->state, -1);
  default:
    WARN("%s should be a string, using default value (%s)", name, def);
    return def;
  }
}

void config_free(Config *config)
{
  assert(config != NULL);
  lua_close(config->state);
  free(config);
}

