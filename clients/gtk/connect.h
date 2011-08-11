#ifndef CONNECT_H
#define CONNECT_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkbutton.h>

G_BEGIN_DECLS

#define CONNECT_TYPE            (connect_get_type())
#define CONNECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CONNECT_TYPE, Connect))
#define CONNECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), CONNECT_TYPE, ConnectClass))
#define IS_CONNECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CONNECT_TYPE))
#define IS_CONNECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CONNECT_TYPE))

typedef struct Connect {
  GtkVBox vbox;

  GtkWidget *username_hbox, *username_label, *username_entry;
  GtkWidget *server_hbox, *server_label, *server_entry;
  GtkWidget *button;
} Connect;

typedef struct ConnectClass {
  GtkVBoxClass parent_class;

  void (* connect) (Connect *connect);
} ConnectClass;

GType connect_get_type(void);
GtkWidget *connect_new(void);

G_END_DECLS

#endif /* CONNECT_H */
