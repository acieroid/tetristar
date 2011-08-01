#ifndef PLUGINS_H
#define PLUGINS_H

#include <glib.h>

#include "global_state.h"
#include "configuration.h"
#include "lua_plugin.h"

void plugins_init();
#if 0
void plugins_on_connect(Plugins *plugins, int id);
void plugins_on_disconnect(Plugins *plugins, int id);
void plugins_on_recv(Plugins *plugins, int id, char *command, char *args);
#endif
void plugins_deinit();

#endif /* PLUGINS_H */

