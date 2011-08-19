#ifndef NETWORK_H
#define NETWORK_H

#include <glib.h>
#include <enet/enet.h>
#include <libtetris.h>

#include "configuration.h"
#include "config.h"

#ifdef ENET_VERSION
  /* Before ENet 1.3, ENET_VERSION was an enum value, and since ENet
     1.3 it's defined as a macro. Since ENet 1.3 isn't compatible with
     older versions, we'd like to know if we're using a "recent"
     version of ENet (ie. >= 1.3) */
  #define HAS_RECENT_ENET
#endif

/**
 * A client
 */
typedef ENetPeer Client;

/**
 * A "network": a server and all its clients
 */
typedef struct Network {
  ENetHost *server;
  GSList *clients;
} Network;

/**
 * Initialize the network
 */
void network_init();

/**
 * Free everything used by the network
 */
void network_deinit();

/**
 * Add a client to the server's client list
 */
void network_add_client(Client *client);

/**
 * Remove a client from the server's client list
 */
void network_remove_client(Client *client);

/**
 * Find a client in the server's client list
 */
Client *network_find_client(int id);

/**
 * Send some data to a client
 */
void network_send(Client *client, const gchar *string);

/**
 * Send some data to all the clients
 */
void network_send_to_all(const gchar *string);


/**
 * Handle the next arriving packet, waiting wait_time
 */
void network_handle_packet(int wait_time);

/**
 * Launch the server loop
 */
void network_loop();

#endif /* NETWORK_H */
