#include "chat.h"

enum {
  NEW_LINE,
  COMMAND_PASSWORD,
  COMMAND_START,
  LAST_SIGNAL
};

static void chat_class_init(ChatClass *klass);
static void chat_init(Chat *chat);
static gboolean chat_on_keypress(GtkWidget *entry,
                                 GdkEventKey *event,
                                 gpointer data);

static guint chat_signals[LAST_SIGNAL] = { 0 };

static struct {
  int signal;
  gchar *command;
} commands[] =   {
  { COMMAND_PASSWORD, "password" },
  { COMMAND_START, "start" },
  { 0, NULL }
};

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
  int i;

  chat_signals[NEW_LINE] =
    g_signal_new("new-line", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(ChatClass, chat),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1,
                 G_TYPE_STRING);
  for (i = 0; commands[i].command != NULL; i++)
    chat_signals[commands[i].signal] =
      g_signal_new(commands[i].command, G_TYPE_FROM_CLASS(klass),
                   G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                   G_STRUCT_OFFSET(ChatClass, chat),
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

  chat->history = NULL;
  chat->history_current = NULL;
  chat->in_history = FALSE;
}

gboolean chat_on_keypress(GtkWidget *entry,
                          GdkEventKey *event,
                          gpointer data)
{
  Chat *chat = (Chat *) data;
  gchar *text;
  int signal = NEW_LINE;
  int i;
  int text_size, command_size;

  if (event->type == GDK_KEY_PRESS) {
    switch (event->keyval) {
    case GDK_KEY_Return:
      chat->in_history = FALSE; /* we are not anymore navigating the history */

      text = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
      text_size = strlen(text);

      /* don't care about empty lines */
      if (text_size == 0)
        return TRUE;

      /* add the text to the history */
      chat->history = g_list_prepend(chat->history, text);
      g_debug("New item in history: %s", text);

      /* is it a command */
      if (text[0] == '/') {
        for (i = 0; commands[i].command != NULL; i++) {
          command_size = strlen(commands[i].command);
          if (text_size < command_size)
            continue;

          if (g_ascii_strncasecmp(text+1, commands[i].command,
                                  command_size) == 0) {
            signal = commands[i].signal;
            /*           '/'  command       ' ' args... */
            text = text + 1 + command_size + 1;
          }
        }
      }

      g_signal_emit(chat, chat_signals[signal], 0, text);
      /* don't free because text is used in the signal handler
         AND in the history */
      gtk_entry_set_text(GTK_ENTRY(entry), "");
      return TRUE;
    case GDK_KEY_Up:
      /* TODO: move all the history-related code in other functions */
      if (chat->history == NULL) {
        /* there's no history, don't do anything */
        return TRUE;
      }
      if (chat->in_history) {
        if (chat->history_current->next == NULL) {
          /* we're at the older entry, so we don't do anything */
          return TRUE;
        }
        /* go to the next entry */
        chat->history_current = chat->history_current->next;
      }
      else {
        /* start navigating the history */
        chat->in_history = TRUE;
        chat->history_current = chat->history;
      }
      gtk_entry_set_text(GTK_ENTRY(entry), chat->history_current->data);
      return TRUE;
    case GDK_KEY_Down:
      if (chat->in_history) {
        if (chat->history_current->prev == NULL) {
          /* already at the most recent entry */
          return TRUE;
        }
        chat->history_current = chat->history_current->prev;
        gtk_entry_set_text(GTK_ENTRY(entry), chat->history_current->data);
      }
      /* else we don't do anything */
      return TRUE;
    case GDK_KEY_Left:
    case GDK_KEY_Right:
      /* we don't want to handle them but we don't want that it
         "breaks" the history navigation neither */
      return FALSE;
    default:
      chat->in_history = FALSE;
      return FALSE;
    }
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

void chat_clear(Chat *chat)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->text_view));
  gtk_text_buffer_set_text(buffer, "", 0);
}
