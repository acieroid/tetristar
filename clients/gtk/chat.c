#include "chat.h"

enum {
  NEW_LINE,
  LAST_SIGNAL
};

static void chat_class_init(ChatClass *klass);
static void chat_init(Chat *chat);

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

    chat_type = g_type_register_static(GTK_TYPE_VBOX, "Chat",
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
                 g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void chat_init(Chat *chat)
{
  gtk_box_set_homogeneous(GTK_BOX(chat), TRUE);
  gtk_box_set_spacing(GTK_BOX(chat), 1);

  chat->text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(chat->text_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat->text_view), FALSE);
  gtk_box_pack_start(GTK_BOX(chat), chat->text_view, TRUE, TRUE, 0);

  chat->entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(chat), chat->entry, TRUE, TRUE, 0);
}

GtkWidget *chat_new(void)
{
  return GTK_WIDGET(g_object_new(CHAT_TYPE, NULL));
}

void chat_add_line(Chat *chat, const gchar *nick, const gchar *line)
{
  gchar *text;
  GtkTextIter end;
  GtkTextBuffer *buffer;

  text = g_strdup_printf("<%s> %s\n", nick, line);

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->text_view));
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_insert(buffer, &end, text, -1);

  g_free(text);
}
