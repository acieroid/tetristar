#include "global_state.h"
#include "configuration.h"
#include "config.h"
#include "network.h"
#include "plugins.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char *configfile;

  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  DBG("Initializing global state");
  global_state_init();
  DBG("Initializing configuration");
  config_init(configfile);
  DBG("Initializing plugins");
  plugins_init();
  DBG("Initializing network");
  network_init();

  network_loop();

  network_deinit();
  plugins_deinit();
  config_deinit();
  global_state_deinit();

  return 0;
}
