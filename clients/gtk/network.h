#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <enet/enet.h>

#define WAIT_TIME_FOR_CONNECTION 5000

typedef struct Network {
  ENetHost *client;
  ENetAddress address;
  ENetPeer *peer;
  gchar *nick;
  int connected;
} Network;

typedef void *(*PthreadFunc) (void*);

Network *network_new(const gchar *server, int port, const gchar *nick);
void network_free(Network *network);
void network_connect(Network *network);
int network_is_connected(Network *network);
void network_send(Network *network, gchar *string);
void network_loop(Network *network);

#endif /* NETWORK_H */
