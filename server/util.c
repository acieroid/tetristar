#include "util.h"

inline void WARN(const char *fmt, ...)
{
  va_list ap;
  fprintf(stderr, "[warning]: ");
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
}

inline void FATAL(const char *fmt, ...)
{
  va_list ap;
  fprintf(stderr, "[fatal]: ");
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(1);
}

inline void DBG(const char *fmt, ...)
{
#ifdef DEBUG
  va_list ap;
  fprintf(stderr, "[debug]: ");
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
#endif
}

static int *ids = NULL;
int new_id()
{
  int i, size;

  size = config_get_int("max_clients", default_max_clients);
  /* keep a track of assigned and free ids */
  if (ids == NULL) {
    ids = malloc(size*sizeof(*ids));
    assert(ids != NULL);
    for (i = 0; i < size; i++) {
      ids[i] = 0;
    }
  }

  /* find the first free id */
  for (i = 0; i < size; i++) {
    if (ids[i] == 0) {
      ids[i] = 1;
      return i;
    }
  }

  /* no more ids available */
  return -1;
}

void free_id(int id)
{
  assert(ids != NULL);
  ids[id] = 0;
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
