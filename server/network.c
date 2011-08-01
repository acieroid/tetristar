#include "network.h"

Network *network_init()
{
  ENetAddress address;
  int status;
  Network *network = malloc(sizeof(*network));
  const char *server = config_get_string("server", default_server);
  int port = config_get_int("port", default_port);
  int max_clients = config_get_int("max_clients", default_max_clients);

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

void network_loop(Network *network)
{
  ENetEvent event;
  assert(network != NULL);
  while (1) {
    enet_host_service(network->server, &event, 1000);
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      printf("New client  connected from %x:%u\n", event.peer->address.host,
             event.peer->address.port);
      event.peer->data = "lol";
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      printf("A packet of length %u containing %s was received from %s on channel %u\n",
             event.packet->dataLength,
             (char *) event.packet->data,
             (char *) event.peer->data,
             event.channelID);
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      printf("%s disconnected\n", (char *) event.peer->data);
      event.peer->data = NULL;
      break;
    case ENET_EVENT_TYPE_NONE:
      printf("Nothing happened\n");
      break;
    }
  }
}

void network_free(Network *network)
{
  assert(network != NULL);
  DBG("Stopping server");
  enet_host_destroy(network->server);
  enet_deinitialize();
}
