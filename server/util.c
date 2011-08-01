#include "util.h"

int gen_id()
{
  static int id = 0;
  return id++;
}

void extract_command(const char *string, char **command, char **args)
{
  int i, len;

  assert(string != NULL);

  len = strlen(string);

  for (i = 0; string[i] != '\0' && string[i] != ' '; i++)
    ;

  *command = malloc(i*sizeof(**command));
  strncpy(*command, string, i);
  args = malloc(len-i*sizeof(**args));
  strncpy(*args, string+i, len-i);
}
