#ifndef NETWORK_H
#define NETWORK_H

#include <assert.h>

#include <enet/enet.h>
#include <glib.h>

#include "plugins.h"
#include "configuration.h"
#include "config.h"
#include "util.h"

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
