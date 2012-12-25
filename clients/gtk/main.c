#include <glib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <libtetris.h>

#include "mainwindow.h"
#include "network.h"
#include "plugins.h"

static MainWindow *mainwindow = NULL;
static lua_State *lua_state = NULL;

void quit_everything(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  g_debug("Shutting down plugins");
  plugins_deinit();

  g_debug("Destroying main window");
  mainwindow_free(mainwindow);

  g_debug("Quitting gtk");
  gtk_main_quit();

  g_debug("Shutting down libtetris");
  tetris_plugin_deinit();
  tetris_deinit();

  g_debug("Shutting down lua");
  lua_close(lua_state);
}

int main(int argc, char *argv[])
{
  g_debug("Initializing lua");
  lua_state = luaL_newstate();
  luaL_openlibs(lua_state);

  g_debug("Initializing libtetris");
  tetris_init();
  tetris_plugin_init(lua_state);
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
  plugins_init(lua_state, mainwindow);

  g_debug("Loading config.lua");
  if (luaL_loadfile(lua_state, "config.lua") != 0 ||
      lua_pcall(lua_state, 0, 0, 0) != 0)
    g_warning("Unable to load or run config.lua: %s",
              lua_tostring(lua_state, -1));

  g_debug("Launching main loop");
  gtk_main();

  return 0;
}
