/**
 * @file network.h
 * @brief Implement the client side of the network
 */
#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <gdk/gdk.h>
#include <enet/enet.h>
#include <libtetris.h>

#include "command.h"

G_BEGIN_DECLS

#define NETWORK_TYPE            (network_get_type())
#define NETWORK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), NETWORK_TYPE, Network))
#define NETWORK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), NETWORK_TYPE, NetworkClass))
#define IS_NETWORK(obj)         (G_TYPE _CHECK_INSTANCE_TYPE((obj), NETWORK_TYPE))
#define IS_NETWORK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NETWORK_TYPE))

#define WAIT_TIME_FOR_CONNECTION 5000

#ifdef ENET_VERSION
  /* See server/network.h for an explanation of this */
  #define HAS_RECENT_ENET
#endif

typedef struct Network {
  GObject parent_instance;

  ENetHost *client;
  ENetAddress address;
  ENetPeer *peer;
  gchar *nick;
  int connected;
} Network;

typedef struct NetworkClass {
  GObjectClass parent_class;

  void (* network) (Network *network);
} NetworkClass;

GType network_get_type(void);
Network *network_new();
void network_free(Network *network);

/**
 * Set the server and port to connect to
 */
void network_set_host(Network *network, const gchar *server, int port);

/**
 * Set the nick of this client
 */
void network_set_nick(Network *network, const gchar *nick);

/**
 * Connect to the server (set with network_set_host) with the client's
 * nick (set with network_set_nick)
 */
void network_connect(Network *network);

/**
 * Check if the client is connected
 */
gboolean network_is_connected(Network *network);

/**
 * Send a string to the server
 */
void network_send(Network *network, gchar *string);

/**
 * Launch the network loop
 */
void network_loop(Network *network);

G_END_DECLS

#endif /* NETWORK_H */
