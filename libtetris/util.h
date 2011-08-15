#ifndef LIBTETRIS_UTIL_H
#define LIBTETRIS_UTIL_H

#include <assert.h>
#include <string.h>

#include <glib.h>

void tetris_extract_command(const gchar *str, int len, gchar **command, gchar **args);

#endif /* LIBTETRIS_UTIL_H */
