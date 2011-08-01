#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <enet/enet.h>

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

int gen_id();

void extract_command(ENetPacket *packet, char **command, char **args);

#endif /* UTIL_H */
