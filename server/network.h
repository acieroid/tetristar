#ifndef NETWORK_H
#define NETWORK_H

#include <enet/enet.h>
#include <assert.h>

#include "configuration.h"
#include "config.h"
#include "util.h"

typedef struct Network {
  Config *config;
  ENetHost *server;
} Network;

Network *network_init(Config *config);
void network_free(Network *network);

#endif /* NETWORK_H */
