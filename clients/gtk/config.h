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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <glib.h>

/**
 * Store the current configuration. Can save it and load it from a
 * file in XDG_CONFIG_DIR
 */
typedef struct Config {
  gchar *dir; /* The dir where the config is saved (XDG_CONFIG_DIR) */
  gchar *filename; /* The file to which the config is saved */

  /* The data saved in the config file */
  gchar *host;
  int port;
  gchar *nick;
} Config;

/**
 * Create a new config. Initialize the data with the default values
 */
Config *config_new(const gchar *filename);
void config_free(Config *config);

/**
 * Save the config on disk. Return true on success.
 */
gboolean config_save(Config *config);

/**
 * Load the config from disk. On failure, it might change some fields
 * of the configuration (those which were successfully read from the
 * file), and return false.
 */
gboolean config_load(Config *config);

/**
 * Set the config's host to a copy of the given string.
 */
void config_set_host(Config *config, const gchar *host);

/**
 * Return the config's host. The returned value should not be freed.
 */
gchar *config_get_host(Config *config);

/**
 * Set the config's port.
 */
void config_set_port(Config *config, int port);

/**
 * Return the config's port.
 */
int config_get_port(Config *config);

/**
 * Set the config's nick to a copy of the given string
 */
void config_set_nick(Config *config, const gchar *nick);

/**
 * Return the config's nick. The returned value should not be freed
 */
gchar *config_get_nick(Config *config);

#endif /* CONFIG_H */
