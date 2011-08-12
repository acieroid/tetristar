#include <stdlib.h>
#include <pthread.h>
#include <gtk/gtk.h>

#include "network.h"
#include "connect.h"

void launch_network(GtkWidget *widget, void *data)
{
  pthread_t thread;
  Connect *connect = CONNECT(widget);
  Network *network = network_new(connect_get_server(connect),
                                 connect_get_port(connect),
                                 connect_get_nick(connect));
  pthread_create(&thread, NULL, (PthreadFunc) network_loop, (void *) network);
}

void remove_connect(GtkWidget *connect, void *data)
{
  GtkWidget *container = (GtkWidget *) data;
  gtk_container_remove(GTK_CONTAINER(container), connect);
}

int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *connect;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Tetristar GTK Client");
  g_signal_connect(G_OBJECT(window), "delete-event",
                   G_CALLBACK(gtk_main_quit), NULL);

  connect = connect_new();
  g_signal_connect(G_OBJECT(connect), "connect",
                   G_CALLBACK(launch_network), NULL);
  g_signal_connect(G_OBJECT(connect), "connect",
                   G_CALLBACK(remove_connect), window);
  gtk_container_add(GTK_CONTAINER(window), connect);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
