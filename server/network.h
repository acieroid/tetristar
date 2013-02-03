/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file network.h
 * @brief Implements everything related to the network
 */
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
