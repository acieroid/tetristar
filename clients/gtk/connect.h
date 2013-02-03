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
 * @file connect.h
 * @brief Implement the "connect" widget, displayed when the client is
 * not connected
 */
#ifndef CONNECT_H
#define CONNECT_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CONNECT_TYPE            (connect_get_type())
#define CONNECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CONNECT_TYPE, Connect))
#define CONNECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), CONNECT_TYPE, ConnectClass))
#define IS_CONNECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CONNECT_TYPE))
#define IS_CONNECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CONNECT_TYPE))

#define DEFAULT_SERVER "localhost"
#define DEFAULT_PORT 12345

typedef struct Connect {
  GtkVBox vbox;

  GtkWidget *nick_hbox, *nick_label, *nick_entry;
  GtkWidget *server_hbox, *server_label, *server_entry;
  GtkWidget *port_hbox, *port_label, *port_spin;
  GtkWidget *button;
} Connect;

typedef struct ConnectClass {
  GtkVBoxClass parent_class;

  void (* connect) (Connect *connect);
} ConnectClass;

GType connect_get_type(void);
GtkWidget *connect_new(void);

/**
 * Return the nick entered by the user
 */
const gchar *connect_get_nick(Connect *connect);

/**
 * Return the server address entered by the user
 */
const gchar *connect_get_server(Connect *connect);

/**
 * Return the port entered by the user
 */
int connect_get_port(Connect *connect);

/**
 * Lock the "connect" button on the widget (to avoid multiple network
 * threads being launched
 */
void connect_lock_button(Connect *connect);

/**
 * Unlock the "connect" button
 */
void connect_unlock_button(Connect *connect);

/**
 * Grab the focus
 */
void connect_grab_focus(Connect *connect);

G_END_DECLS

#endif /* CONNECT_H */
