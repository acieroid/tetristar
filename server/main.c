#include "configuration.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  Config *config;
  char *configfile;
  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  config = config_init(configfile);

  printf("Listening on %s:%d\n", config_get_string(config, "server", "localhost"),
         config_get_int(config, "port", 12345));

  config_free(config);
}
