#include "network.h"

Network *network_init(const char *server, int port)
{
  ENetAddress address;
  int status;
  Network *network = malloc(sizeof(*network));
  assert(network != NULL);

  status = enet_initialize();
  assert(status == 0);

  enet_address_set_host(&address, server);
  address.port = port;

  network->server = enet_host_create(&address, max_clients, 0, 0);
  assert(network->server != NULL);

  DBG("Listening on %s:%d", server, port);

  return network;
}

void network_free(Network *network)
{
  assert(network != NULL);
  DBG("Stopping server");
  enet_host_destroy(network->server);
  enet_deinitialize();
}
  