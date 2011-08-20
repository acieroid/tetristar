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

G_END_DECLS

#endif /* CONNECT_H */
