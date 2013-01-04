#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <glib.h>
#include <gtk/gtk.h>
#include <libtetris.h>

#include "chat.h"
#include "connect.h"
#include "context.h"
#include "network.h"

typedef struct MainWindow {
  GtkWidget *window;
  GtkWidget *connect;
  GtkWidget *chat;
  GtkWidget *context;
  GtkWidget *connected_vbox, *main_vbox;

  GtkWidget *toolbar;
  GtkToolItem *button_disconnect, *button_play, *button_pause, *button_stop;

  Network *network;
  int connected;
} MainWindow;

MainWindow *mainwindow_new(void);
void mainwindow_free(MainWindow *window);

Context *mainwindow_get_context(MainWindow *mw);

/**
 * Display an error dialog with the given message
 */
void display_error(const gchar *message);

#endif /* MAINWINDOW_H */
