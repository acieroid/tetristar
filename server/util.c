#include "util.h"

int gen_id()
{
  static int id = 0;
  return id++;
}

void extract_command(ENetPacket *packet, char **command, char **args)
{
  int i, len = packet->dataLength;
  const char *string = (const char *) packet->data;

  assert(string != NULL);

  for (i = 0; string[i] != '\0' && string[i] != ' '; i++)
    ;

  *command = malloc(i*sizeof(**command));
  assert(*command != NULL);
  strncpy(*command, string, i);

  *args = malloc(len-i*sizeof(**args));
  assert(*args != NULL);
  strncpy(*args, string+i, len-i);
}
