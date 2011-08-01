#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <enet/enet.h>

#include "configuration.h"
#include "config.h"

#define WARN(...)                               \
  {                                             \
    fprintf(stderr, "[warning] " __VA_ARGS__);  \
    fprintf(stderr, "\n");                      \
  }

#define FATAL(...)                                      \
  {                                                     \
    fprintf(stderr, "[fatal] " __VA_ARGS__);            \
    fprintf(stderr, "\n");                              \
    exit(1);                                            \
  }

#define DBG(...)                                \
  {                                             \
    fprintf(stderr, "[debug] " __VA_ARGS__);    \
    fprintf(stderr, "\n");                      \
  }

int new_id();
void free_id(int id);
void extract_command(ENetPacket *packet, char **command, char **args);

#endif /* UTIL_H */
