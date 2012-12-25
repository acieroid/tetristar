#include <signal.h>

#include <glib.h>
#include <libtetris.h>

#include "configuration.h"
#include "config.h"
#include "network.h"
#include "plugins.h"

static lua_State *lua_state;

void tetristar_init(gchar *configfile)
{
  g_debug("Initializing tetristar server");

  g_debug("Inilializing lua");
  lua_state = luaL_newstate();
  luaL_openlibs(lua_state);

  g_debug("Initializing configuration");
  config_init(lua_state, configfile);

  g_debug("Initializing libtetris");
  tetris_init();
  tetris_plugin_init(lua_state);
  tetris_lua_functions_setup();
  tetris_id_init(config_get_int("max_clients",
                                default_max_clients));

  g_debug("Initializing server plugins");
  plugins_init();

  g_debug("Initializing network");
  network_init();
}

void tetristar_deinit()
{
  g_debug("Shutting down tetristar server");

  g_debug("Shutting down network");
  network_deinit();

  g_debug("Shutting down server plugins");
  plugins_deinit();

  g_debug("Shutting down libtetris");
  tetris_deinit();
  tetris_plugin_deinit();
  tetris_id_deinit();

  g_debug("Shutting down configuration");
  config_deinit();

  g_debug("Shutting down lua");
  lua_close(lua_state);
}

void catch_signal(int signum)
{
  switch (signum) {
  case SIGINT:
    g_message("Exiting tetristar...");
    tetristar_deinit();
    exit(0);
  case SIGUSR1:
    g_message("Reloading plugins");
    tetris_plugin_unload_all();
    plugins_load_all();
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[])
{
  gchar *configfile;
  signal(SIGINT, catch_signal);
  signal(SIGUSR1, catch_signal);

  if (argc > 1)
    configfile = g_strdup(argv[1]);
  else
    configfile = g_strdup("config.lua");

  tetristar_init(configfile);
  g_free(configfile);

  network_loop();
  tetristar_deinit();

  return 0;
}
