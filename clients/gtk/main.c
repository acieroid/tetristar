#include <stdlib.h>

#include <gtk/gtk.h>

#include "connect.h"

void foo(GtkWidget *connect, void *data)
{
  printf("%s @ %s:%d\n", connect_get_nick(CONNECT(connect)),
         connect_get_server(CONNECT(connect)), connect_get_port(CONNECT(connect)));
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
                   G_CALLBACK(foo), NULL);
  gtk_container_add(GTK_CONTAINER(window), connect);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
