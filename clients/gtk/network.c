#include "network.h"

enum {
  CONNECTED,
  DISCONNECTED,
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
  network_signals[CONNECTED] =
    g_signal_new("connected", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(NetworkClass, network),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
  network_signals[DISCONNECTED] =
    g_signal_new("disconnected", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(NetworkClass, network),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void network_init(Network *network)
{
  network->client = NULL;
  network->nick = NULL;
  network->peer = NULL;
  network->connected = 0;
}

Network *network_new()
{
  return NETWORK(g_object_new(NETWORK_TYPE, NULL));
}

void network_set_host(Network *network, const gchar *server, int port)
{
  assert(network != NULL);
  enet_address_set_host(&(network->address), (const char *) server);
  network->address.port = port;
}

void network_set_nick(Network *network, const gchar *nick)
{
  if (network->nick != NULL)
    g_free(network->nick);
  network->nick = g_strdup(nick);
}

void network_shutdown(Network *network)
{
  assert(network != NULL);
  enet_peer_disconnect(network->peer, 0);
  network->peer = NULL;
  enet_host_destroy(network->client);
  network->client = NULL;
  if (network->nick != NULL) {
    g_free(network->nick);
    network->nick = NULL;
  }
}

void network_free(Network *network)
{
  network_shutdown(network);
  g_free(network);
}

void network_connect(Network *network)
{
  printf("Connecting network\n");
  ENetEvent event;
  gchar *str;
  assert(network != NULL);

  network->client = enet_host_create(NULL, 1,
#ifdef HAS_RECENT_ENET
                                     2,
#endif
                                     0, 0);
  assert(network->client != NULL);

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
    printf("Network connected\n");
    network->connected = 1;
    str = g_strdup_printf("HELLO %s", network->nick);
    network_send(network, str);
    g_free(str);
  }
  else {
    printf("Can't connect to network, event.type: %d\n", event.type);
  }
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
  gchar *command, *args;
  assert(network != NULL);

  if (!network_is_connected(network))
    network_connect(network);

  while (network_is_connected(network)) {
    enet_host_service(network->client, &event, 1000);
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE:
      tetris_extract_command((const gchar *) event.packet->data,
                             event.packet->dataLength,
                             &command, &args);
      g_printf("> %s - %s\n", command, args);
      if (g_strcmp0(command, "HELLO") == 0) {
        gdk_threads_enter();
        g_signal_emit(network, network_signals[CONNECTED], 0);
        gdk_threads_leave();
      }
      /* TODO: emit recv signal */
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      network->connected = 0;
      gdk_threads_enter();
      g_signal_emit(network, network_signals[DISCONNECTED], 0);
      gdk_threads_leave();
      break;
    default:
      break;
    }
  }
  /* TODO: when to free the network ? */
  network_shutdown(network);
  printf("Network stopped.\n");
}
