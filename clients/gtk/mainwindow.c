#include "mainwindow.h"

static void launch_network(GtkWidget *widget, gpointer data);
static void send_line(Chat *chat, const gchar *line, gpointer data);
static void connected_layout(GtkWidget *widget, gpointer data);
static void disconnected_layout(GtkWidget *widget, gpointer data);
static void unlock_button(GtkWidget *widget, gpointer data);

typedef void *(*PthreadFunc) (void*);

MainWindow *mainwindow_new(void)
{
  MainWindow *window = g_malloc(sizeof(*window));

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

  window->chat = chat_new();
  g_signal_connect(G_OBJECT(window->chat), "new-line",
                   G_CALLBACK(send_line), window);

  window->context = context_new();

  /* we start disconnected */
  window->connected = 0;
  disconnected_layout(NULL, window);

  gtk_widget_show_all(window->window);
  return window;
}

void launch_network(GtkWidget *widget, gpointer data)
{
  pthread_t thread;
  MainWindow *window = (MainWindow *) data;

  network_set_host(window->network,
                   connect_get_server(CONNECT(window->connect)),
                   connect_get_port(CONNECT(window->connect)));
  network_set_nick(window->network,
                   connect_get_nick(CONNECT(window->connect)));

  /* we lock the button to avoid launching multiple network threads */
  connect_lock_button(CONNECT(window->connect));
  pthread_create(&thread, NULL, (PthreadFunc) network_loop,
                 (void *) window->network);
}

void send_line(Chat *chat, const gchar *line, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  gchar *str = g_strdup_printf("SAY %s", line);
  network_send(window->network, str);
}

void connected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  window->connected = 1;
  g_object_ref(window->connect); /* keep a reference */
  gtk_container_remove(GTK_CONTAINER(window->window), window->connect);
  gtk_container_add(GTK_CONTAINER(window->window), window->chat);
  chat_set_focus(CHAT(window->chat));
  gtk_widget_show_all(window->chat);
}

void disconnected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  if (window->connected) {
    window->connected = 0;
    g_object_ref(window->chat);
    gtk_container_remove(GTK_CONTAINER(window->window), window->chat);
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
