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
  char *command, *args;
  assert(network != NULL);
  while (1) {
    enet_host_service(network->server, &event, 1000);
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      event.peer->data = (void *) new_id();
      plugins_on_action(PLUGIN_CONNECT, (int) event.peer->data, NULL, NULL);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      extract_command(event.packet, &command, &args);
      plugins_on_action(PLUGIN_RECV, (int) event.peer->data, command, args);
      free(command);
      free(args);
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      plugins_on_action(PLUGIN_DISCONNECT, (int) event.peer->data, NULL, NULL);
      free_id((int) event.peer->data);
      event.peer->data = NULL;
      break;
    case ENET_EVENT_TYPE_NONE:
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
