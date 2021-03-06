/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mainwindow.h"

static void launch_network(GtkWidget *widget, gpointer data);
static void send_command(Chat *chat, const gchar *args, gpointer data);
static void unknown_command(Chat *chat, const gchar *command, gpointer data);
static void quit_command(Chat *chat, const gchar *args, gpointer data);
static void disconnect_command(Chat *chat, const gchar *args, gpointer data);
static void connected_layout(GtkWidget *widget, gpointer data);
static void disconnected_layout(GtkWidget *widget, gpointer data);
static void unlock_button(GtkWidget *widget, gpointer data);
static void clean_after_disconnect(GtkWidget *widget, gpointer data);
static void error_message(GtkWidget *widget, gpointer data);
static gboolean on_keypress(GtkWidget *widget,
                            GdkEventKey *event,
                            gpointer data);
static gboolean on_keyrelease(GtkWidget *widget,
                              GdkEventKey *event,
                              gpointer data);
static gboolean down_timeout(gpointer data);
static void disconnect_clicked(GtkWidget *widget, gpointer data);
static void play_clicked(GtkWidget *widget, gpointer data);
static void pause_clicked(GtkWidget *widget, gpointer data);
static void stop_clicked(GtkWidget *widget, gpointer data);
static void focus_context(GtkWidget *widget, GtkDirectionType dir, gpointer data);

/* TODO: use a better way to represent keybinds (they can do more than
   just sending simple commands) */
static struct {
  int keyval;
  const gchar *command;
} keybinds[] = {
  { GDK_KEY_Left, "MOVE LEFT" },
  { GDK_KEY_Right, "MOVE RIGHT" },
  { GDK_KEY_Up, "ROTATE RIGHT" },
  { GDK_KEY_space, "DROP" },
  { GDK_KEY_1, "BONUS 1" },
  { GDK_KEY_2, "BONUS 2" },
  { GDK_KEY_3, "BONUS 3" },
  { GDK_KEY_4, "BONUS 4" },
  { GDK_KEY_5, "BONUS 5" },
  { GDK_KEY_6, "BONUS 6" },
  { GDK_KEY_7, "BONUS 7" },
  { GDK_KEY_8, "BONUS 8" },
  { GDK_KEY_9, "BONUS 9" },
  { GDK_KEY_0, "BONUS 10" },
  { GDK_KEY_d, "BONUS 0" }, /* drop bonus */
  { GDK_KEY_c, "KEEP" },
  { GDK_KEY_VoidSymbol, NULL }
};

/* the 'down' action should be treated separately since it requires
   autorepeat */
static const int KEY_DOWN = GDK_KEY_Down;
static const gchar *KEY_DOWN_CMD = "MOVE DOWN";
static const int KEY_DOWN_DELAY = 100;

static struct {
  const gchar *signal;
  const gchar *command;
  gboolean has_args;
} commands[] = {
  { "new-line", "SAY", TRUE },
  { "password", "PASSWORD", TRUE },
  { "start", "START", FALSE },
  { "stop", "STOP", FALSE },
  { "pause", "PAUSE", FALSE },
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
  gtk_window_set_default_size(GTK_WINDOW(window->window),
                              300, 700);

  window->connect = connect_new();
  g_signal_connect(G_OBJECT(window->connect), "connect",
                   G_CALLBACK(launch_network), window);

  window->network = network_new();
  g_signal_connect(G_OBJECT(window->network), "connected",
                   G_CALLBACK(connected_layout), window);
  g_signal_connect(G_OBJECT(window->network), "disconnected",
                   G_CALLBACK(disconnected_layout), window);
  g_signal_connect(G_OBJECT(window->network), "disconnected",
                   G_CALLBACK(error_message), "Connection closed by the server");
  g_signal_connect(G_OBJECT(window->network), "disconnected",
                   G_CALLBACK(clean_after_disconnect), window);
  g_signal_connect(G_OBJECT(window->network), "cant-connect",
                   G_CALLBACK(unlock_button), window);
  g_signal_connect(G_OBJECT(window->network), "cant-connect",
                   G_CALLBACK(error_message), "Cannot connect to the server");

  g_signal_connect(G_OBJECT(window->window), "focus",
                   G_CALLBACK(focus_context), window);

  window->chat = chat_new();
  for (i = 0; commands[i].command != NULL; i++) {
    command_info = g_malloc(sizeof(*command_info));
    command_info->command_id = i;
    command_info->window = window;
    g_signal_connect(G_OBJECT(window->chat), commands[i].signal,
                     G_CALLBACK(send_command), command_info);
  }

  g_signal_connect(G_OBJECT(window->chat), "disconnect",
                   G_CALLBACK(disconnect_command), window);
  g_signal_connect(G_OBJECT(window->chat), "quit",
                   G_CALLBACK(quit_command), window);

  g_signal_connect(G_OBJECT(window->chat), "unknown-command",
                   G_CALLBACK(unknown_command), NULL);

  window->context = context_new();
  window->down_pressed = FALSE;
  g_signal_connect(G_OBJECT(window->context), "key-press-event",
                   G_CALLBACK(on_keypress), window);
  g_signal_connect(G_OBJECT(window->context), "key-release-event",
                   G_CALLBACK(on_keyrelease), window);
  window->connected_pane = gtk_vpaned_new();
  gtk_paned_pack1(GTK_PANED(window->connected_pane), window->context, TRUE, FALSE);
  gtk_paned_pack2(GTK_PANED(window->connected_pane), window->chat, FALSE, FALSE);

  window->toolbar = gtk_toolbar_new();

  i = 0;
  window->button_disconnect = gtk_tool_button_new_from_stock(GTK_STOCK_DISCONNECT);
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(window->button_disconnect), "Disconnect");
  g_signal_connect(G_OBJECT(window->button_disconnect), "clicked",
                   G_CALLBACK(disconnect_clicked), window);
  gtk_toolbar_insert(GTK_TOOLBAR(window->toolbar), window->button_disconnect, i++);

  window->button_play = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(window->button_play), "Play");
  g_signal_connect(G_OBJECT(window->button_play), "clicked",
                   G_CALLBACK(play_clicked), window);
  gtk_toolbar_insert(GTK_TOOLBAR(window->toolbar), window->button_play, i++);

  window->button_pause = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PAUSE);
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(window->button_pause), "Pause");
  g_signal_connect(G_OBJECT(window->button_pause), "clicked",
                   G_CALLBACK(pause_clicked), window);
  gtk_toolbar_insert(GTK_TOOLBAR(window->toolbar), window->button_pause, i++);

  window->button_stop = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(window->button_stop), "Stop");
  g_signal_connect(G_OBJECT(window->button_stop), "clicked",
                   G_CALLBACK(stop_clicked), window);
  gtk_toolbar_insert(GTK_TOOLBAR(window->toolbar), window->button_stop, i++);

  window->main_vbox = gtk_vbox_new(FALSE, 1);
  gtk_box_pack_start(GTK_BOX(window->main_vbox), window->toolbar, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(window->window), window->main_vbox);
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
  gtk_widget_destroy(window->connected_pane);
  gtk_widget_destroy(GTK_WIDGET(window->button_disconnect));
  gtk_widget_destroy(GTK_WIDGET(window->button_play));
  gtk_widget_destroy(GTK_WIDGET(window->button_pause));
  gtk_widget_destroy(GTK_WIDGET(window->button_stop));
  gtk_widget_destroy(window->toolbar);
  gtk_widget_destroy(window->main_vbox);
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
#if GLIB_CHECK_VERSION(2,32,0)
  g_thread_new("network",
               (GThreadFunc) network_loop,
               (gpointer) window->network);
#else
  g_thread_init(NULL);
  g_thread_create((GThreadFunc) network_loop,
                  (gpointer) window->network,
                  FALSE, NULL);
#endif
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
  /* Do no free str since it is kept in the history */
}

void unknown_command(Chat *chat, const gchar *command, gpointer data)
{
  /* drop the args */
  gchar *space = strchr(command, ' ');
  if (space != NULL) {
    *space = '\0';
  }

  chat_add_colored_text(chat, "error", ">>> Unknown command: %s\n", command);
}

void disconnect_command(Chat *chat, const gchar *args, gpointer data)
{
  disconnect_clicked(GTK_WIDGET(chat), data);
}

void quit_command(Chat *chat, const gchar *args, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  gtk_widget_destroy(GTK_WIDGET(window->window));
  gtk_main_quit();
}

void connected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  window->connected = 1;
  g_object_ref(window->connect); /* keep a reference */
  gtk_container_remove(GTK_CONTAINER(window->main_vbox), window->connect);
  gtk_container_add(GTK_CONTAINER(window->main_vbox), window->connected_pane);
  chat_set_focus(CHAT(window->chat));
  gtk_widget_show_all(window->connected_pane);

  gtk_widget_set_sensitive(GTK_WIDGET(window->button_disconnect), TRUE);
}

void disconnected_layout(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  if (window->connected) {
    window->connected = 0;
    g_object_ref(window->connected_pane);
    gtk_container_remove(GTK_CONTAINER(window->main_vbox), window->connected_pane);
  }
  connect_unlock_button(CONNECT(window->connect));
  gtk_container_add(GTK_CONTAINER(window->main_vbox), window->connect);
  gtk_widget_show_all(window->connect);
  connect_grab_focus(CONNECT(window->connect));

  gtk_widget_set_sensitive(GTK_WIDGET(window->button_disconnect), FALSE);
}

void unlock_button(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  connect_unlock_button(CONNECT(window->connect));
}

void clean_after_disconnect(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;

  context_remove_all_players(CONTEXT(window->context));
  chat_clear(CHAT(window->chat));
}

void error_message(GtkWidget *widget, gpointer data)
{
  const gchar *message = (gchar *) data;
  display_error(message);
}

void display_error(const gchar *message)
{
  GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "%s", message);
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
    if (event->keyval == GDK_KEY_Tab) {
      chat_set_focus(CHAT(window->chat));
      return TRUE;
    } else if (event->keyval == KEY_DOWN && !window->down_pressed) {
      window->down_pressed = TRUE;
      network_send(window->network, (gchar *) KEY_DOWN_CMD);
      g_timeout_add(KEY_DOWN_DELAY, down_timeout, window);
    }
    for (i = 0; keybinds[i].command != NULL; i++) {
      if (event->keyval == keybinds[i].keyval) {
        network_send(window->network, (gchar *) keybinds[i].command);
        return TRUE;
      }
    }
  }
  return FALSE;
}

gboolean on_keyrelease(GtkWidget *widget,
                       GdkEventKey *event,
                       gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  if (event->keyval == KEY_DOWN && window->down_pressed) {
    window->down_pressed = FALSE;
  }
  return TRUE;
}

gboolean down_timeout(gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  if (window->down_pressed) {
    network_send(window->network, (gchar *) KEY_DOWN_CMD);
    return TRUE;
  }
  return FALSE; /* the timeout should be removed */
}

void disconnect_clicked(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  disconnected_layout(NULL, window);
  network_shutdown(window->network);
  clean_after_disconnect(NULL, window);
}

void play_clicked(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  network_send(window->network, "START");
}

void pause_clicked(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  network_send(window->network, "PAUSE");
}

void stop_clicked(GtkWidget *widget, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  network_send(window->network, "STOP");
}

void focus_context(GtkWidget *widget, GtkDirectionType dir, gpointer data)
{
  MainWindow *window = (MainWindow *) data;
  context_grab_focus(CONTEXT(window->context));
}
