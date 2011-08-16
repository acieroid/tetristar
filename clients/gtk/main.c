#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <glib.h>
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

typedef void *(*PthreadFunc) (void*);

void launch_network(GtkWidget *widget, void *data)
{
  printf("Launching network\n");
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

void connected_layout(GtkWidget *widget, void *data)
{
  printf("Connected!\n");
  MainWindow *window = (MainWindow *) data;
  window->connected = 1;
  g_object_ref(window->connect); /* keep a reference */
  gtk_container_remove(GTK_CONTAINER(window->window), window->connect);
  gtk_container_add(GTK_CONTAINER(window->window), window->chat);
  gtk_widget_show_all(window->chat);
  chat_add_line(CHAT(window->chat), "system", "Connected.");
}

void disconnected_layout(GtkWidget *widget, void *data)
{
  printf("Disconnected!\n");
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

void unlock_button(GtkWidget *widget, void *data)
{
  MainWindow *window = (MainWindow *) data;
  connect_unlock_button(CONNECT(window->connect));
}

void foo(GtkWidget *widget, gchar *args, void *data)
{
  printf("New player: %s\n", args);
}

int main(int argc, char *argv[])
{
  MainWindow *window = malloc(sizeof(*window));
  assert(window != NULL);

  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc, &argv);

  window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window->window), "Tetristar GTK Client");
  g_signal_connect(G_OBJECT(window->window), "delete-event",
                   G_CALLBACK(gtk_main_quit), NULL);

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
  g_signal_connect(G_OBJECT(window->network), "NEWPLAYER",
                   G_CALLBACK(foo), window);

  window->chat = chat_new();

  /* we start disconnected */
  window->connected = 0;
  disconnected_layout(NULL, window);

  gtk_widget_show_all(window->window);
  gtk_main();

  return 0;
}
