#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libtetris.h>

#include "global_state.h"
#include "configuration.h"
#include "config.h"
#include "network.h"
#include "plugins.h"

void tetristar_init(char *configfile)
{
  DBG("Initializing tetristar server");
  global_state_init();
  config_init(configfile);
  tetris_init();
  tetris_setup_lua(LUA_STATE);
  plugins_init();
  network_init();
}

void tetristar_deinit()
{
  DBG("Quitting tetristar server");
  network_deinit();
  plugins_deinit();
  tetris_deinit();
  config_deinit();
  global_state_deinit();
}

void catch_sigint(int signum)
{
  tetristar_deinit();
  exit(0);
}

int main(int argc, char *argv[])
{
  char *configfile;
  signal(SIGINT, catch_sigint);

  if (argc > 1)
    configfile = argv[1];
  else
    configfile = strdup("conf.lua");

  tetristar_init(configfile);
  if (argc <= 1)
    free(configfile);

  network_loop();
  tetristar_deinit();

  return 0;
}
