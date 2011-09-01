/**
 * @file util.h
 * Various utilities
 */
#ifndef LIBTETRIS_UTIL_H
#define LIBTETRIS_UTIL_H

#include <unistd.h>
#include <string.h>

#include <glib.h>

/**
 * Initializes the ids, setting the maximum id to max_id
 */
void tetris_id_init(int max_id);

/**
 * Free everything related to the ids
 */
void tetris_id_deinit();

/**
 * Get a free id, or -1 if there are no more free ids
 */
int tetris_id_new();

/**
 * Free an used id
 */
void tetris_id_free(int id);

/**
 * Extract a command from a string.
 * @param str contains the string from which extract the command
 * @param len is the length of the string (if -1, the length will of
 * the string will be calculated using strlen)
 * @param command is a pointer to a string (that will be allocated in
 * this function) that will contain the command
 * @param args is a pointer to a string (that will also be allocated
 * in this function) that will contain the command's arguments
 */
void tetris_extract_command(const gchar *str,
                            int len,
                            gchar **command,
                            gchar **args);

/**
 * Caclulate the greatest common divisor of two numbers
 */
int tetris_gcd(int a, int b);

/**
 * Sleep for the given next microseconds
 */
void tetris_usleep(guint microseconds);

#endif /* LIBTETRIS_UTIL_H */
