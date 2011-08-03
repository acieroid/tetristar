#include <stdio.h>
#include <string.h>

#include <libtetris.h>

#include "global_state.h"
#include "configuration.h"
#include "config.h"
#include "network.h"
#include "plugins.h"

int main(int argc, char *argv[])
{
  char *configfile;

  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  global_state_init();
  config_init(configfile);
  DBG("Init tetris");
  tetris_init();
  DBG("Init tetris lua");
  tetris_setup_lua(LUA_STATE);
  DBG("Done");
  plugins_init();
  network_init();

  network_loop();

  network_deinit();
  plugins_deinit();
  tetris_deinit();
  config_deinit();
  global_state_deinit();

  return 0;
}
