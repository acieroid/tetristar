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

#include "util.h"

static int *ids = NULL;
static int max_id = 0;

void tetris_id_init(int n)
{
  int i;
  max_id = n;

  if (ids != NULL)
    g_free(ids);

  /* keep a track of assigned and free ids */
  ids = g_malloc(max_id*sizeof(*ids));
  for (i = 0; i < max_id; i++)
    ids[i] = 0;
}

void tetris_id_deinit()
{
  g_free(ids);
}

int tetris_id_new()
{
  int i;

  /* find the first free id */
  for (i = 0; i < max_id; i++) {
    if (ids[i] == 0) {
      ids[i] = 1;
      return i+1;
    }
  }

  /* no more ids available */
  g_return_val_if_reached(-1);
}

void tetris_id_free(int id)
{
  g_return_if_fail(ids != NULL);
  ids[id-1] = 0;
}

void tetris_extract_command(const gchar *str,
                            int len,
                            gchar **command,
                            gchar **args)
{
  int i;
  g_return_if_fail(str != NULL);

  if (len == -1)
    len = strlen(str);

  for (i = 0; str[i] != '\0' && str[i] != ' '; i++)
    ;

  *command = g_strndup(str, i+1);
  (*command)[i] = '\0';

  i++;
  *args = g_strndup(str+i, len-i);
}

int tetris_gcd(int a, int b)
{
  int tmp;
  while (b != 0) {
    tmp = b;
    b = a % b;
    a = tmp;
  }
  return a;
}

void tetris_usleep(guint microseconds)
{
  usleep(microseconds);
}
