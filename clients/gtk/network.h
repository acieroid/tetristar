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
 * Shutdown the network (in a way that in can be reconnected with
 * network_connect)
 */
void network_shutdown(Network *network);

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
