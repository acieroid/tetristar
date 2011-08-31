#include "chat.h"

enum {
  NEW_LINE,
  LAST_SIGNAL
};

static void chat_class_init(ChatClass *klass);
static void chat_init(Chat *chat);
static gboolean chat_on_keypress(GtkWidget *entry,
                                 GdkEventKey *event,
                                 gpointer data);

static guint chat_signals[LAST_SIGNAL] = { 0 };

GType chat_get_type(void)
{
  static GType chat_type = 0;

  if (!chat_type) {
    const GTypeInfo chat_info = {
      sizeof(ChatClass), NULL, NULL,
      (GClassInitFunc) chat_class_init, NULL, NULL,
      sizeof(Chat), 0,
      (GInstanceInitFunc) chat_init,
    };

    chat_type = g_type_register_static(GTK_TYPE_TABLE, "Chat",
                                       &chat_info, 0);
  }

  return chat_type;
}

void chat_class_init(ChatClass *klass)
{
  chat_signals[NEW_LINE] =
    g_signal_new("new-line", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET (ChatClass, chat),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1,
                 G_TYPE_STRING);
}

void chat_init(Chat *chat)
{
  gtk_table_resize(GTK_TABLE(chat), 3, 3);

  chat->text_view = gtk_text_view_new();
  gtk_widget_set_can_focus(chat->text_view, FALSE);

  gtk_text_view_set_editable(GTK_TEXT_VIEW(chat->text_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat->text_view), FALSE);
  gtk_table_attach(GTK_TABLE(chat), chat->text_view, 0, 1, 0, 1,
                   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

  chat->entry = gtk_entry_new();
  gtk_signal_connect(GTK_OBJECT(chat->entry), "key-press-event",
                     G_CALLBACK(chat_on_keypress), chat);
  gtk_widget_add_events (chat->entry, GDK_KEY_PRESS_MASK);
  gtk_table_attach(GTK_TABLE(chat), chat->entry, 0, 1, 1, 2,
                   GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
}

gboolean chat_on_keypress(GtkWidget *entry,
                          GdkEventKey *event,
                          gpointer data)
{
  Chat *chat = (Chat *) data;
  if (event->type == GDK_KEY_PRESS &&
      event->keyval == GDK_KEY_Return) {
    g_signal_emit(chat, chat_signals[NEW_LINE], 0,
                  gtk_entry_get_text(GTK_ENTRY(entry)));
    gtk_entry_set_text(GTK_ENTRY(entry), "");
    return TRUE;
  }
  return FALSE;
}

GtkWidget *chat_new(void)
{
  return GTK_WIDGET(g_object_new(CHAT_TYPE, NULL));
}

void chat_set_focus(Chat *chat)
{
  gtk_widget_grab_focus(chat->entry);
}

void chat_add_text(Chat *chat, const gchar *format, ...)
{
  va_list ap;
  gchar *text;
  GtkTextIter end;
  GtkTextBuffer *buffer;

  va_start(ap, format);
  text = g_strdup_vprintf(format, ap);
  va_end(ap);

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->text_view));
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_insert(buffer, &end, text, -1);

  g_free(text);
}
