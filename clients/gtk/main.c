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
  gdk_threads_init();
  gtk_init(&argc, &argv);

  g_debug("Creating main window");
  mainwindow = mainwindow_new();
  g_signal_connect(G_OBJECT(mainwindow->window), "delete-event",
                   G_CALLBACK(quit_everything), NULL);
  g_signal_connect(G_OBJECT(mainwindow->window), "destroy-event",
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
