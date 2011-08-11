#include "connect.h"

enum {
  CONNECTED_SIGNAL,
  LAST_SIGNAL
};

static void connect_class_init(ConnectClass *klass);
static void connect_init(Connect *connect);

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
                 G_STRUCT_OFFSET (ConnectClass, connect),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void connect_init(Connect *connect)
{
  gtk_box_set_homogeneous(GTK_BOX(connect), TRUE);
  gtk_box_set_spacing(GTK_BOX(connect), 1);

  connect->username_hbox = gtk_hbox_new(TRUE, 1);
  connect->username_label = gtk_label_new("Username");
  connect->username_entry = gtk_entry_new();
  gtk_label_set_mnemonic_widget(GTK_LABEL(connect->username_label),
                                connect->username_entry);
  gtk_box_pack_start(GTK_BOX(connect->username_hbox), connect->username_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect->username_hbox), connect->username_entry,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect), connect->username_hbox, TRUE, TRUE, 0);

  connect->server_hbox = gtk_hbox_new(TRUE, 1);
  connect->server_label = gtk_label_new("Server");
  connect->server_entry = gtk_entry_new();
  gtk_label_set_mnemonic_widget(GTK_LABEL(connect->server_label),
                                connect->server_entry);
  gtk_box_pack_start(GTK_BOX(connect->server_hbox), connect->server_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect->server_hbox), connect->server_entry,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(connect), connect->server_hbox, TRUE, TRUE, 0);

  connect->button = gtk_button_new_with_label("Connect");
  gtk_box_pack_start(GTK_BOX(connect), connect->button, TRUE, TRUE, 0);
}

GtkWidget *connect_new(void)
{
  return GTK_WIDGET(g_object_new(CONNECT_TYPE, NULL));
}
