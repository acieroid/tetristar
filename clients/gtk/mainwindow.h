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
  GtkWidget *vbox;
  Network *network;
  int connected;
} MainWindow;

MainWindow *mainwindow_new(void);

Context *mainwindow_get_context(MainWindow *mw);

#endif /* MAINWINDOW_H */
