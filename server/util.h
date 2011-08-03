#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <enet/enet.h>

#include "configuration.h"
#include "config.h"

void WARN(const char *fmt, ...);
void FATAL(const char *fmt, ...);
void DBG(const char *fmt, ...);

int new_id();
void free_id(int id);
void extract_command(ENetPacket *packet, char **command, char **args);

#endif /* UTIL_H */
