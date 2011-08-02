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
  do {                                          \
    fprintf(stderr, "[warning] " __VA_ARGS__);  \
    fprintf(stderr, "\n");                      \
  } while (0)

#define FATAL(...)                                      \
  do {                                                  \
    fprintf(stderr, "[fatal] " __VA_ARGS__);            \
    fprintf(stderr, "\n");                              \
    exit(1);                                            \
  } while (0)

#define DBG(...)                                \
  do {                                          \
    fprintf(stderr, "[debug] " __VA_ARGS__);    \
    fprintf(stderr, "\n");                      \
  } while(0)

int new_id();
void free_id(int id);
void extract_command(ENetPacket *packet, char **command, char **args);

#endif /* UTIL_H */
