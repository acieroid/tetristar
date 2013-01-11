#include "network.h"

static Network *network;

void network_init()
{
  ENetAddress address;
  int status, port, max_clients;
  gchar *server;

  network = g_malloc(sizeof(*network));

  server = (gchar *) config_get_string("server", default_server);
  port = config_get_int("port", default_port);
  max_clients = config_get_int("max_clients", default_max_clients);

  status = enet_initialize();
  g_return_if_fail(status == 0);

  enet_address_set_host(&address, server);
  address.port = port;

  network->server = enet_host_create(&address, max_clients, 
#ifdef HAS_RECENT_ENET
                                     2, 
#endif
                                     0, 0);
  g_return_if_fail(network->server != NULL);
  g_debug("Listening on %s:%d", server, port);

  network->clients = NULL;
  g_free(server);
}

void network_deinit()
{
  g_debug("Stopping server");
  tetris_plugin_action(PLUGIN_SHUTDOWN, -1, NULL, NULL);

  /* Disconnect each clients */
  g_slist_foreach(network->clients, (GFunc) enet_peer_disconnect_now, 0);

  /* ENet's documentation doesn't say anything about how to properly
    delete an ENetPeer, and using free on them results in a segfault,
    so we just don't do anything */
  g_slist_free(network->clients);
  enet_host_destroy(network->server);
  enet_deinitialize();
  g_free(network);
}

void network_add_client(Client *client)
{
  network->clients = g_slist_prepend(network->clients, client);
}

void network_remove_client(Client *client)
{
  network->clients = g_slist_remove(network->clients, client);
}

Client *network_find_client(int id)
{
  GSList *elem;
  Client *client;

  for (elem = network->clients; elem != NULL; elem = elem->next) {
    client = (Client *) elem->data;
    if ((int) client->data == id)
      return client;
  }
  g_return_val_if_reached(NULL);
}

void network_handle_packet(int wait_time)
{
  ENetEvent event;
  gchar *command, *args;

  enet_host_service(network->server, &event, wait_time);
  switch (event.type) {
  case ENET_EVENT_TYPE_CONNECT:
    event.peer->data = (void *) tetris_id_new();
    tetris_plugin_action(PLUGIN_CONNECT, (int) event.peer->data,
                         NULL, NULL);
    network_add_client(event.peer);
    break;
  case ENET_EVENT_TYPE_RECEIVE:
    tetris_extract_command((const gchar *) event.packet->data,
                           event.packet->dataLength,
                           &command, &args);
    tetris_plugin_action(PLUGIN_RECV, (int) event.peer->data,
                         command, args);
    g_free(command);
    g_free(args);
    enet_packet_destroy(event.packet);
    break;
  case ENET_EVENT_TYPE_DISCONNECT:
    tetris_plugin_action(PLUGIN_DISCONNECT, (int) event.peer->data,
                         NULL, NULL);
    network_remove_client(event.peer);
    tetris_id_free((int) event.peer->data);
    event.peer->data = NULL;
    break;
  case ENET_EVENT_TYPE_NONE:
    break;
  }
}

void network_loop()
{
  while (1) {
    network_handle_packet(1000);
  }
}

void network_send(Client *client, const char *string)
{
  ENetPacket *packet;
  g_return_if_fail(string != NULL);
  packet = enet_packet_create(string, strlen(string) + 1,
                              ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(client, 0, packet);
  enet_host_flush(network->server);
}

void network_send_to_all(const char *string)
{
  g_slist_foreach(network->clients, (GFunc) network_send,
                  (void *) string);
}
 
