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

#include "config.h"

#define CONFIG_BUFSIZE 1024

static const char *DEFAULT_HOST = "localhost";
static const int DEFAULT_PORT = 12345;
static const char *DEFAULT_NICK = "TetristarPlayer";

Config *config_new(const gchar *filename)
{
  Config *config = g_malloc(sizeof(*config));
  config->dir = g_strdup(getenv("XDG_CONFIG_HOME"));
  config->filename = g_strdup(filename);

  config->host = g_strdup(DEFAULT_HOST);
  config->port = DEFAULT_PORT;
  config->nick = g_strdup(DEFAULT_NICK);

  return config;
}

void config_free(Config *config)
{
  g_free(config->dir);
  g_free(config->filename);

  if (config->host != NULL) {
    g_free(config->host);
  }

  if (config->nick != NULL) {
    g_free(config->nick);
  }
}

gboolean config_save(Config *config)
{
  gchar *path;
  FILE *f;

  path = g_strdup_printf("%s/%s", config->dir, config->filename);
  f = fopen(path, "w");
  g_free(path);

  if (f == NULL) {
    g_warning("Cannot save configuration file: %s", strerror(errno));
    return FALSE;
  }

  fprintf(f, "host=%s\n", config->host);
  fprintf(f, "port=%d\n", config->port);
  fprintf(f, "nick=%s\n", config->nick);

  if (fclose(f) != 0) {
    g_warning("Cannot close configuration file: %s", strerror(errno));
    return FALSE;
  }

  return TRUE;
}

gboolean config_load(Config *config)
{
  gchar *path;
  char buf[CONFIG_BUFSIZE];
  FILE *f;
  int n, port;

  path = g_strdup_printf("%s/%s", config->dir, config->filename);
  f = fopen(path, "r");
  g_free(path);

  if (f == NULL) {
    g_warning("Cannot load configuration file: %s", strerror(errno));
    return FALSE;
  }

  while (fgets(buf, CONFIG_BUFSIZE, f) != 0) {
    n = strlen(buf);
    if (n <= 1) {
      continue;
    }
    buf[n-1] = '\0'; /* drop the \n or the EOF*/
    if (strncmp(buf, "host=", 5) == 0) {
      if (config->host != NULL) {
        g_free(config->host);
      }
      config->host = g_strdup(buf+5);
    } else if (strncmp(buf, "port=", 5) == 0) {
      port = strtol(buf+5, NULL, 10);
      if (port <= 0 || port > (2 << 16) - 1) {
        g_warning("Invalid port: %s", buf);
        continue;
      }
      config->port = port;
    } else if (strncmp(buf, "nick=", 5) == 0) {
      if (config->nick != NULL) {
        g_free(config->nick);
      }
      config->nick = g_strdup(buf+5);
    } else {
      g_warning("Ignoring invalid configuration line: %s", buf);
    }
  }

  if (ferror(f)) {
    g_warning("An error has occured while reading the configuration file");
    clearerr(f);
  }

  if (fclose(f) != 0) {
    g_warning("Cannot close configuration file: %s", strerror(errno));
    return FALSE;
  }

  return TRUE;
}

void config_set_host(Config *config, const gchar *host)
{
  if (config->host) {
    g_free(config->host);
  }
  config->host = g_strdup(host);
}

gchar *config_get_host(Config *config)
{
  return config->host;
}

void config_set_port(Config *config, int port)
{
  config->port = port;
}

int config_get_port(Config *config)
{
  return config->port;
}

void config_set_nick(Config *config, const gchar *nick)
{
  if (config->nick) {
    g_free(config->nick);
  }
  config->nick = g_strdup(nick);
}

gchar *config_get_nick(Config *config)
{
  return config->nick;
}
