#include "network.h"

void network_init()
{
  ENetAddress address;
  int status, port, max_clients;
  char *server;

  assert(global_state != NULL);
  NETWORK = malloc(sizeof(*NETWORK));
  assert(NETWORK != NULL);

  server = (char *) config_get_string("server", default_server);
  port = config_get_int("port", default_port);
  max_clients = config_get_int("max_clients", default_max_clients);

  status = enet_initialize();
  assert(status == 0);

  enet_address_set_host(&address, server);
  address.port = port;

  NETWORK->server = enet_host_create(&address, max_clients, 
#if HAS_RECENT_ENET
                                     2, 
#endif
                                     0, 0);
  assert(NETWORK->server != NULL);
  DBG("Listening on %s:%d", server, port);

  NETWORK->clients = NULL;
  free(server);
}

void network_deinit()
{
  assert(NETWORK != NULL);
  DBG("Stopping server");
  /* ENet's documentation doesn't say anything about how to properly 
    delete an ENetPeer, and using free on them results in a segfault,
    so we just don't do anything */
  g_slist_free(NETWORK->clients);
  enet_host_destroy(NETWORK->server);
  enet_deinitialize();
  free(NETWORK);
}

void network_add_client(Client *client)
{
  NETWORK->clients = g_slist_prepend(NETWORK->clients, client);
}

void network_remove_client(Client *client)
{
  NETWORK->clients = g_slist_remove(NETWORK->clients, client);
}

Client *network_find_client(int id)
{
  GSList *elem;
  Client *client;

  for (elem = NETWORK->clients; elem != NULL; elem = elem->next) {
    client = (Client *) elem->data;
    if ((int) client->data == id)
      return client;
  }
  WARN("Client wih id %d not found", id);
  return NULL;
}

void network_loop()
{
  ENetEvent event;
  char *command, *args;
  assert(NETWORK != NULL);
  while (1) {
    enet_host_service(NETWORK->server, &event, 1000);
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      event.peer->data = (void *) new_id();
      plugins_on_action(PLUGIN_CONNECT, (int) event.peer->data, NULL, NULL);
      network_add_client(event.peer);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      extract_command(event.packet, &command, &args);
      printf("Data received: '%s' - '%s'\n", command, args);
      plugins_on_action(PLUGIN_RECV, (int) event.peer->data, command, args);
      free(command);
      free(args);
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      plugins_on_action(PLUGIN_DISCONNECT, (int) event.peer->data, NULL, NULL);
      network_remove_client(event.peer);
      free_id((int) event.peer->data);
      event.peer->data = NULL;
      break;
    case ENET_EVENT_TYPE_NONE:
      break;
    }
  }
}

void network_send(Client *client, const char *string)
{
  ENetPacket *packet;
  assert(string != NULL);
  packet = enet_packet_create(string, strlen(string) + 1,
                              ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(client, 0, packet);
  enet_host_flush(NETWORK->server);
}

void network_send_to_all(const char *string)
{
  g_slist_foreach(NETWORK->clients, (GFunc) network_send, (void *) string);
}
 
