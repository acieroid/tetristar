#include "util.h"

int new_id()
{
  int i, size;

  size = config_get_int("max_clients", default_max_clients);
  /* keep a track of assigned and free ids */
  if (global_state->ids == NULL) {
    global_state->ids = malloc(size*sizeof(*global_state->ids));
    assert(global_state->ids != NULL);
    for (i = 0; i < size; i++) {
      global_state->ids[i] = 0;
    }
  }

  /* find the first free id */
  for (i = 0; i < size; i++) {
    if (global_state->ids[i] == 0) {
      global_state->ids[i] = 1;
      return i;
    }
  }

  /* no more ids available */
  return -1;
}

void free_id(int id)
{
  assert(global_state != NULL);
  assert(global_state->ids != NULL);

  global_state->ids[id] = 0;
}

void extract_command(ENetPacket *packet, char **command, char **args)
{
  int i, len = packet->dataLength;
  const char *string = (const char *) packet->data;

  assert(string != NULL);

  for (i = 0; string[i] != '\0' && string[i] != ' '; i++)
    ;

  *command = malloc(i+1*sizeof(**command));
  assert(*command != NULL);
  strncpy(*command, string, i);
  (*command)[i] = '\0';

  i++;
  *args = malloc(len-i*sizeof(**args));
  assert(*args != NULL);
  strncpy(*args, string+i, len-i);
}
