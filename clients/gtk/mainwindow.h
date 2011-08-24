#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <glib.h>
#include <gtk/gtk.h>
#include <libtetris.h>
#include <pthread.h>

#include "chat.h"
#include "connect.h"
#include "context.h"
#include "network.h"

typedef struct MainWindow {
  GtkWidget *window;
  GtkWidget *connect;
  GtkWidget *chat;
  GtkWidget *context;
  Network *network;
  int connected;
} MainWindow;

MainWindow *mainwindow_new(void);

#endif /* MAINWINDOW_H */
