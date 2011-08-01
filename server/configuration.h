#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <assert.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glib.h>

#include "global_state.h"
#include "util.h"

void config_init(const char *file);
int config_get_int(const char *name, int def);
const char *config_get_string(const char *name, const char *def);
GSList *config_get_list(const char *name, GSList *def);
void config_deinit();

#endif /* CONFIGURATION_H */
