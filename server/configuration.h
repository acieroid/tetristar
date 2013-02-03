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

/**
 * @file configuration.h
 * @brief Implements the configuration module, which read a config
 * file and extract its parameters
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
