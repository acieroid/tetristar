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
