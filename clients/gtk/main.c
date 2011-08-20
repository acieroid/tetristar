#include <glib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <libtetris.h>

#include "mainwindow.h"
#include "network.h"
#include "plugins.h"

void quit_everything(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  tetris_deinit();
  /* TODO: stop & free the network */
  gtk_main_quit();
}

int main(int argc, char *argv[])
{
  MainWindow *mainwindow;

  g_debug("Initializing lua");
  lua_State *l = lua_open();
  luaL_openlibs(l);

  g_debug("Initializing libtetris");
  tetris_init();
  tetris_plugin_init(l);
  tetris_lua_functions_setup();

  g_debug("Initializing GLib and GTK");
  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc, &argv);

  g_debug("Creating main window");
  mainwindow = mainwindow_new();
  g_signal_connect(G_OBJECT(mainwindow->window), "delete-event",
                   G_CALLBACK(quit_everything), NULL);

  g_debug("Initializing clients plugins");
  plugins_init(l, mainwindow);

  g_debug("Loading config.lua");
  if (luaL_loadfile(l, "config.lua") != 0 ||
      lua_pcall(l, 0, 0, 0) != 0)
    g_warning("Unable to load or run config.lua: %s",
              lua_tostring(l, -1));

  g_debug("Launching main loop");
  gtk_main();

  return 0;
}
