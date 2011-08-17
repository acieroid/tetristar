#ifndef CHAT_H
#define CHAT_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

#define CHAT_TYPE            (chat_get_type())
#define CHAT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CHAT_TYPE, Chat))
#define CHAT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST(klass), CHAT_TYPE, Chat))
#define IS_CHAT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CHAT_TYPE))
#define IS_CHAT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE(klass), CHAT_TYPE)

typedef struct Chat {
  GtkTable table;
  GtkWidget *text_view;
  GtkWidget *entry;
} Chat;

typedef struct ChatClass {
  GtkTableClass parent_class;

  void (* chat) (Chat *chat);
} ChatClass;

GType chat_get_type(void);
GtkWidget *chat_new(void);

void chat_add_line(Chat *chat, const gchar *format, ...);

G_END_DECLS

#endif /* CHAT_H */
