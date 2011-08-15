#include "util.h"

void tetris_extract_command(const gchar *str, int len, gchar **command, gchar **args)
{
  int i;
  assert(str != NULL);

  if (len == -1)
    len = strlen(str);

  for (i = 0; str[i] != '\0' && str[i] != ' '; i++)
    ;

  *command = g_strndup(str, i+1);
  (*command)[i] = '\0';

  i++;
  *args = g_strndup(str+i, len-i);
}
