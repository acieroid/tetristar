#include "plugins.h"

void plugins_init()
{
  global_state->plugins = NULL;
  g_slist_foreach(config_get_list("plugins", NULL), lua_plugin_load, NULL);
}

void plugins_deinit()
{
  g_slist_foreach(PLUGINS, lua_plugin_free, NULL);
}
