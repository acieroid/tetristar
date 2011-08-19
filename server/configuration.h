/**
 * @file configuration.h
 * Implements the configuration module, which read a config file and
 * extract its parameters
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glib.h>

#include "util.h"

/**
 * Initialize the configuration: load the configuration file
 */
void config_init(lua_State *l, const gchar *file);

/**
 * Free the data used by this module
 */
void config_deinit();

/**
 * Return an integer defined in the configuration file, or some
 * default value
 */
int config_get_int(const gchar *name, int def);

/**
 * Return a string defined in the configuration file, or some default
 * value
 */
const gchar *config_get_string(const gchar *name, const gchar *def);

/**
 * Return a list defined in the configuration file, or some default
 * value
 */
GSList *config_get_list(const gchar *name, GSList *def);

#endif /* CONFIGURATION_H */
