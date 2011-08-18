/**
 * @file plugins.h
 * Implements the network's plugins
 */
#ifndef PLUGINS_H
#define PLUGINS_H

#include <glib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <libtetris.h>

#include "configuration.h"
#include "network.h"
#include "util.h"

/**
 * Initializes the network's plugins
 */
void plugins_init();

/**
 * Deinitializes the network's plugins
 */
void plugins_deinit();

#endif /* PLUGINS_H */

