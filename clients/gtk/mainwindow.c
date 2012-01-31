#include "mainwindow.h"

static void launch_network(GtkWidget *widget, gpointer data);
static void send_command(Chat *chat, const gchar *args, gpointer data);
static void connected_layout(GtkWidget *widget, gpointer data);
static void disconnected_layout(GtkWidget *widget, gpointer data);
static void unlock_button(GtkWidget *widget, gpointer data);
static void cant_connect(GtkWidget *widget, gpointer data);
static gboolean on_keypress(GtkWidget *widget,
                            GdkEventKey *event,
                            gpointer data);

static struct {
  int keyval;
  const gchar *command;
} keybinds[] = {
  { GDK_KEY_Left, "MOVE LEFT" },
  { GDK_KEY_Right, "MOVE RIGHT" },
  { GDK_KEY_Down, "MOVE DOWN" },
  { GDK_KEY_Up, "ROTATE RIGHT" },
  { GDK_KEY_space, "DROP" },
  { GDK_KEY_VoidSymbol, NULL }
};

static struct {
  const gchar *signal;
  const gchar *command;
  gboolean has_args;
} commands[] = {
  { "new-line", "SAY", TRUE },
  { "password", "PASSWORD", TRUE },
  { "start", "START", FALSE },
  { 0, NULL, FALSE }
};

struct CommandInfo {
  MainWindow *window;
  int command_id;
};

MainWindow *mainwindow_new(void)
{
  int i;
  MainWindow *window = g_malloc(sizeof(*window));
  struct CommandInfo *command_info;

  window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window->window),
                       "Tetristar GTK Client");

  window->connect = connect_new();
  g_signal_connect(G_OBJECT(window->connect), "connect",
                   G_CALLBACK(launch_network), window);

  window->network = network_new();
  g_signal_connect(G_OBJECT(window->network), "connected",
                   G_CALLBACK(connected_layout), window);
  g_signal_connect(G_OBJECT(window->network), "disconnected",
                   G_CALLBACK(disconnected_layout), window);
  g_signal_connect(G_OBJECT(window->network), "cant-connect",
                   G_CALLBACK(unlock_button), window);
  g_signal_connect(G_OBJECT(window->network), "cant-connect",
                   G_CALLBACK(cant_connect), window);

  window->chat = chat_new();
  for (i = 0; commands[i].command != NULL; i++) {
    command_info = g_malloc(sizeof(*command_info));
    command_info->command_id = i;
    command_info->window = window;
    g_signal_connect(G_OBJECT(window->chat), commands[i].signal,
                     G_CALLBACK(send_command), command_info);
  }

  window->context = context_new();
  g_signal_connect(G_OBJECT(window->context), "key-press-event",
                   G_CALLBACK(on_keypress), window);

  window->vbox = gtk_vbox_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window->vbox), window->context);
  gtk_container_add(GTK_CONTAINER(window->vbox), window->chat);

  /* we start disconnected */
  window->connected = 0;
  disconnected_layout(NULL, window);

  gtk_widget_show_all(window->window);
  return window;
}

void mainwindow_free(MainWindow *window)
{
  gtk_widget_destroy(window->connect);
  gtk_widget_destroy(window->chat);
  context_remove_all_players(CONTEXT(window->context));
  gtk_widget_destroy(window->context);
  gtk_widget_destroy(window->vbox);
  gtk_widget_destroy(window->window);
  if (network_is_connected(window->network)) {
    network_send(window->network, "BYE");
    while (network_is_connected(window->network))
      tetris_usleep(1000000);
  }
  network_free(window->network);
}

void launch_network(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;

  network_set_host(window->network,
                   connect_get_server(CONNECT(window->connect)),
                   connect_get_port(CONNECT(window->connect)));
  network_set_nick(window->network,
                   connect_get_nick(CONNECT(window->connect)));

  /* we lock the button to avoid launching multiple network threads */
  connect_lock_button(CONNECT(window->connect));
  g_thread_create((GThreadFunc) network_loop,
                  (gpointer) window->network,
                  FALSE, NULL);
}

void send_command(Chat *chat, const gchar *args, gpointer data)
{
  struct CommandInfo *command_info = (struct CommandInfo *) data;
  gchar *str;

  if (commands[command_info->command_id].has_args)
    str = g_strdup_printf("%s %s",
                          commands[command_info->command_id].command,
                          args);
  else
    str = g_strdup_printf("%s", commands[command_info->command_id].command);

  network_send(command_info->window->network, str);
  g_free(str);
}

void connected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  window->connected = 1;
  g_object_ref(window->connect); /* keep a reference */
  gtk_container_remove(GTK_CONTAINER(window->window), window->connect);
  gtk_container_add(GTK_CONTAINER(window->window), window->vbox);
  chat_set_focus(CHAT(window->chat));
  gtk_widget_show_all(window->vbox);
}

void disconnected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  if (window->connected) {
    window->connected = 0;
    g_object_ref(window->vbox);
    gtk_container_remove(GTK_CONTAINER(window->window), window->vbox);
  }
  connect_unlock_button(CONNECT(window->connect));
  gtk_container_add(GTK_CONTAINER(window->window), window->connect);
  gtk_widget_show_all(window->connect);
}

void unlock_button(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  connect_unlock_button(CONNECT(window->connect));
}

void cant_connect(GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "Cannot connect to server");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

gboolean on_keypress(GtkWidget *widget,
                     GdkEventKey *event,
                     gpointer data)
{
  int i;
  MainWindow *window = (MainWindow *) data;

  if (event->type == GDK_KEY_PRESS) {
    for (i = 0; keybinds[i].command != NULL; i++) {
      if (event->keyval == keybinds[i].keyval) {
        network_send(window->network, (gchar *) keybinds[i].command);
        return TRUE;
      }
    }
  }
  return FALSE;
}
