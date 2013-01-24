#include "util.h"

static int *ids = NULL;
static int max_id = 1;

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
      return i;
    }
  }

  /* no more ids available */
  g_return_val_if_reached(-1);
}

void tetris_id_free(int id)
{
  g_return_if_fail(ids != NULL);
  ids[id] = 0;
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
