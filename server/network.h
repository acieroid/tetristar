#ifndef NETWORK_H
#define NETWORK_H

#include <enet/enet.h>
#include <assert.h>

#include "plugins.h"
#include "configuration.h"
#include "config.h"
#include "util.h"

typedef struct Network {
  ENetHost *server;
} Network;

Network *network_init();
void network_loop(Network *network);
void network_free(Network *network);

#endif /* NETWORK_H */
