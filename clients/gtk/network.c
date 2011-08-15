#include "network.h"

enum {
  NEWPLAYER,
  SAY,
  LAST_SIGNAL
};

static struct {
  int signal;
  const gchar *command;
} signals[] = {
  { NEWPLAYER, "NEWPLAYER" },
  { SAY, "SAY" },
};

static void network_class_init(NetworkClass *klass);
static void network_init(Network *network);

static guint network_signals[LAST_SIGNAL] = { 0 };

GType network_get_type(void)
{
  static GType network_type = 0;

  if (!network_type) {
    const GTypeInfo network_info = {
      sizeof(NetworkClass), NULL, NULL,
      (GClassInitFunc) network_class_init, NULL, NULL,
      sizeof(Network), 0,
      (GInstanceInitFunc) network_init,
    };

    network_type = g_type_register_static (G_TYPE_OBJECT, "Network",
                                           &network_info, 0);
  }

  return network_type;
}

void network_class_init(NetworkClass *klass)
{
}

void network_init(Network *network)
{
  network->client = enet_host_create(NULL, 1, 
#ifdef HAS_RECENT_ENET
                                     2, 
#endif
                                     0, 0);
  assert(network->client != NULL);

  network->nick = NULL;
  network->peer = NULL;
  network->connected = 0;
}

Network *network_new(const gchar *server, int port, const gchar *nick)
{
  Network *network = NETWORK(g_object_new(NETWORK_TYPE, NULL));
  assert(network != NULL);

  network_set_host(network, server, port);
  network_set_nick(network, nick);
  return network;
}

void network_set_host(Network *network, const gchar *server, int port)
{
  assert(network != NULL);
  enet_address_set_host(&(network->address), server);
  network->address.port = port;
}

void network_set_nick(Network *network, const gchar *nick)
{
  if (network->nick != NULL)
    g_free(network->nick);
  network->nick = g_strdup(nick);
}

void network_free(Network *network)
{
  assert(network != NULL);
  enet_peer_disconnect(network->peer, 0);
  enet_host_destroy(network->client);
  if (network->nick != NULL)
    g_free(network->nick);
  g_free(network);
}

void network_connect(Network *network)
{
  ENetEvent event;
  gchar *str;
  assert(network != NULL);

  if (network->peer == NULL)
    network->peer = 
        enet_host_connect(network->client, &(network->address), 2
#ifdef HAS_RECENT_ENET 
                          , 0
#endif
                          );

  assert(network->peer != NULL);

  if (enet_host_service(network->client, &event, WAIT_TIME_FOR_CONNECTION) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    network->connected = 1;
  }

  str = g_strdup_printf("HELLO %s", network->nick);
  network_send(network, str);
  g_free(str);
}
    
int network_is_connected(Network *network)
{
  assert(network != NULL);
  return network->connected;
}

void network_send(Network *network, gchar *string)
{
  ENetPacket *packet;
  assert(network != NULL);
  packet = enet_packet_create(string, strlen(string)+1,
                              ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(network->peer, 0, packet);
  enet_host_flush(network->client);
}

void network_loop(Network *network)
{
  ENetEvent event;
  assert(network != NULL);

  if (!network_is_connected(network))
    network_connect(network);

  while (1) {
    enet_host_service(network->client, &event, 1000);
    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      g_printf("> %s\n", event.packet->data);
    }
    else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
      g_printf("Disconnected\n");
      break;
    }
  }
  /* we're disconnected, so we can free the structure */
  network_free(network);
}
