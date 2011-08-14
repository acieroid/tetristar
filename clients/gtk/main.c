#include <stdlib.h>
#include <pthread.h>
#include <gtk/gtk.h>

#include "network.h"
#include "connect.h"
#include "chat.h"

typedef struct MainWindow {
  GtkWidget *window;
  GtkWidget *connect;
  GtkWidget *chat;
  Network *network;
  int connected;
} MainWindow;

void launch_network(GtkWidget *widget, void *data)
{
  pthread_t thread;
  Connect *connect = CONNECT(widget);
  Network *network = network_new(connect_get_server(connect),
                                 connect_get_port(connect),
                                 connect_get_nick(connect));

  pthread_create(&thread, NULL, (PthreadFunc) network_loop, (void *) network);
}

void connected_layout(GtkWidget *widget, void *data)
{
  MainWindow *window = (MainWindow *) data;
  window->connected = 1;
  gtk_container_remove(GTK_CONTAINER(window->window), window->connect);
  gtk_container_add(GTK_CONTAINER(window->window), window->chat);
  gtk_widget_show_all(window->chat);
  chat_add_line(window->chat, "system", "Connected.");
}

void disconnected_layout(GtkWidget *widget, void *data)
{
  MainWindow *window = (MainWindow *) data;
  if (window->connected) {
    window->connected = 0;
    gtk_container_remove(GTK_CONTAINER(window->window), window->chat);
  }
  gtk_container_add(GTK_CONTAINER(window->window), window->connect);
  gtk_widget_show_all(window->connect);
}

int main(int argc, char *argv[])
{
  MainWindow *window = malloc(sizeof(*window));
  assert(window != NULL);

  gtk_init(&argc, &argv);

  window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window->window), "Tetristar GTK Client");
  g_signal_connect(G_OBJECT(window->window), "delete-event",
                   G_CALLBACK(gtk_main_quit), NULL);

  window->connect = connect_new();
  g_signal_connect(G_OBJECT(window->connect), "connect",
                   G_CALLBACK(launch_network), window);
  g_signal_connect(G_OBJECT(window->connect), "connect",
                   G_CALLBACK(connected_layout), window);

  window->chat = chat_new();

  /* we start disconnected */
  window->connected = 0;
  disconnected_layout(NULL, window);

  gtk_widget_show_all(window->window);
  gtk_main();

  return 0;
}
