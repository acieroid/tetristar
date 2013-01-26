/**
 * @file chat.h
 * @brief Implement the irc-like chat widget
 */
#ifndef CHAT_H
#define CHAT_H

#include <string.h>

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
  /* Widget */
  GtkTable table;
  GtkWidget *scroll;
  GtkWidget *text_view;
  GtkWidget *entry;

  /* History management */
  GList *history, *history_current;
  gboolean in_history;
  
} Chat;

typedef struct ChatClass {
  GtkTableClass parent_class;

  void (* chat) (Chat *chat);
} ChatClass;

GType chat_get_type(void);
GtkWidget *chat_new(void);

/**
 * Set the focus to the chat's entry. To be called when the chat is
 * added to a container
 */
void chat_set_focus(Chat *chat);

/**
 * Add some text to the chat
 */
void chat_add_text(Chat *chat, const gchar *format, ...);

/**
 * Clear all the data from the chat
 */
void chat_clear(Chat *chat);

G_END_DECLS

#endif /* CHAT_H */
