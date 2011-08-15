#ifndef NETWORK_H
#define NETWORK_H

#include <assert.h>

#include <enet/enet.h>
#include <glib.h>
#include <libtetris.h>

#include "plugins.h"
#include "configuration.h"
#include "config.h"
#include "util.h"

#ifdef ENET_VERSION
  /* Before ENet 1.3, ENET_VERSION was an enum value, and since ENet
     1.3 it's defined as a macro. Since ENet 1.3 isn't compatible with
     older versions, we'd like to know if we're using a "recent"
     version of ENet (ie. >= 1.3) */
  #define HAS_RECENT_ENET
#endif

typedef ENetPeer Client;

void network_init();
void network_deinit();
void network_add_client(Client *client);
void network_remove_client(Client *client);
Client *network_find_client(int id);
void network_loop();
void network_send(Client *client, const char *string);
void network_send_to_all(const char *string);

#endif /* NETWORK_H */
