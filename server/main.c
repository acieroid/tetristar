#include "global_state.h"
#include "configuration.h"
#include "config.h"
#include "network.h"
#include "plugins.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  Network *network;
  char *configfile;

  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  global_state_init();
  config_init(configfile);
  plugins_init();
  network = network_init();

  network_loop(network);

  network_free(network);
  plugins_deinit();
  config_deinit();
  global_state_deinit();

  return 0;
}
