#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <lua.h>

typedef struct Config {
  lua_State *state;
} Config;

Config *config_init(const char *file);
int config_get_int(Config *config, const char *name, int def);
const char *config_get_string(Config *config, const char *name, char *def);
void config_free(Config *config);

#endif /* CONFIGURATION_H */
