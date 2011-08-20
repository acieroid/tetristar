/**
 * @file plugins.h
 *
 * @brief Implements the functions callable from the plugins and load
 * the plugins
 */
#ifndef PLUGINS_H
#define PLUGINS_H

#include <glib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <libtetris.h>

#include "network.h"
#include "mainwindow.h"

void plugins_init(lua_State *l, MainWindow *mw);
void plugins_deinit();

#endif /* PLUGINS_H */
