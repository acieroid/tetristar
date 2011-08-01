#include "configuration.h"
#include "config.h"
#include "network.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  Config *config;
  Network *network;
  char *configfile;
  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  config = config_init(configfile);
  network = network_init(config);

  config_free(config);
  network_free(network);

  return 0;
}
