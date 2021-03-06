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

#include "connect.h"

enum {
  CONNECTED_SIGNAL,
  LAST_SIGNAL
};

static void connect_class_init(ConnectClass *klass);
static void connect_init(Connect *connect);

static void connect_destroyed(Connect *connect);

static guint connect_signals[LAST_SIGNAL] = { 0 };

GType connect_get_type(void)
{
  static GType connect_type = 0;

  if (!connect_type) {
    const GTypeInfo connect_info = {
      sizeof(ConnectClass), NULL, NULL,
      (GClassInitFunc) connect_class_init, NULL, NULL,
      sizeof(Connect), 0,
      (GInstanceInitFunc) connect_init,
    };

    connect_type = g_type_register_static (GTK_TYPE_VBOX, "Connect",
                                           &connect_info, 0);
  }

  return connect_type;
}

void connect_class_init(ConnectClass *klass)
{
  connect_signals[CONNECTED_SIGNAL] =
    g_signal_new("connect", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(ConnectClass, connect),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void connect_emit(GtkWidget *widget, void *data)
{
  Connect *connect = (Connect *) data;

  config_set_nick(connect->config, connect_get_nick(connect));
  config_set_host(connect->config, connect_get_server(connect));
  config_set_port(connect->config, connect_get_port(connect));
  config_save(connect->config);
  g_signal_emit(connect, connect_signals[CONNECTED_SIGNAL], 0);
}

void connect_init(Connect *connect)
{
  gtk_box_set_homogeneous(GTK_BOX(connect), TRUE);
  gtk_box_set_spacing(GTK_BOX(connect), 1);

  connect->config = config_new("tetristar.conf");
  config_load(connect->config);

  connect->nick_hbox = gtk_hbox_new(TRUE, 1);
  connect->nick_label = gtk_label_new("Nick");
  connect->nick_entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(connect->nick_entry),
                     config_get_nick(connect->config));

  /* note that the GTK manual recommends not to connect to this
     signal, maybe there is a cleaner solution? */
  g_signal_connect(G_OBJECT(connect->nick_entry), "activate",
                   G_CALLBACK(connect_emit), (void *) connect);
  gtk_label_set_mnemonic_widget(GTK_LABEL(connect->nick_label),
                                connect->nick_entry);
  gtk_box_pack_start(GTK_BOX(connect->nick_hbox), connect->nick_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect->nick_hbox), connect->nick_entry,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect), connect->nick_hbox, TRUE, TRUE, 0);

  connect->server_hbox = gtk_hbox_new(TRUE, 1);
  connect->server_label = gtk_label_new("Server");
  connect->server_entry = gtk_entry_new();
  g_signal_connect(G_OBJECT(connect->server_entry), "activate",
                   G_CALLBACK(connect_emit), (gpointer) connect);
  gtk_entry_set_text(GTK_ENTRY(connect->server_entry),
                     config_get_host(connect->config));
  gtk_label_set_mnemonic_widget(GTK_LABEL(connect->server_label),
                                connect->server_entry);
  gtk_box_pack_start(GTK_BOX(connect->server_hbox), connect->server_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect->server_hbox), connect->server_entry,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect), connect->server_hbox, TRUE, TRUE, 0);

  connect->port_hbox = gtk_hbox_new(TRUE, 1);
  connect->port_label = gtk_label_new("Port");
  connect->port_spin = gtk_spin_button_new_with_range(0, (2 << 16) - 1, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(connect->port_spin),
                            config_get_port(connect->config));
  gtk_label_set_mnemonic_widget(GTK_LABEL(connect->port_label),
                                connect->port_spin);
  gtk_box_pack_start(GTK_BOX(connect->port_hbox), connect->port_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect->port_hbox), connect->port_spin,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect), connect->port_hbox, TRUE, TRUE, 0);

  connect->button = gtk_button_new_with_label("Connect");
  g_signal_connect(G_OBJECT(connect->button), "clicked",
                   G_CALLBACK(connect_emit), (gpointer) connect);
  gtk_box_pack_start(GTK_BOX(connect), connect->button, TRUE, TRUE, 0);
  gtk_widget_grab_focus(connect->nick_entry);

  g_signal_connect(G_OBJECT(connect), "destroy",
                   G_CALLBACK(connect_destroyed), (gpointer) connect);
}

GtkWidget *connect_new(void)
{
  return GTK_WIDGET(g_object_new(CONNECT_TYPE, NULL));
}

const gchar *connect_get_nick(Connect *connect)
{
  return gtk_entry_get_text(GTK_ENTRY(connect->nick_entry));
}

const gchar *connect_get_server(Connect *connect)
{
  return gtk_entry_get_text(GTK_ENTRY(connect->server_entry));
}

int connect_get_port(Connect *connect)
{
  return gtk_spin_button_get_value(GTK_SPIN_BUTTON(connect->port_spin));
}

void connect_lock_button(Connect *connect)
{
  gtk_widget_set_sensitive(connect->button, FALSE);
}

void connect_unlock_button(Connect *connect)
{
  gtk_widget_set_sensitive(connect->button, TRUE);
}

void connect_grab_focus(Connect *connect)
{
  gtk_widget_grab_focus(connect->nick_entry);
}

void connect_destroyed(Connect *connect)
{
  config_free(connect->config);
  connect->config = NULL;
}
