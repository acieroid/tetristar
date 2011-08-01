#ifndef PLUGINS_H
#define PLUGINS_H

#include <glib.h>

#include "global_state.h"
#include "configuration.h"
#include "lua_plugin.h"

void plugins_init();
void plugins_deinit();
void plugins_on_action(PluginType type, int id, char *command, char *args);

#endif /* PLUGINS_H */

