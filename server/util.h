#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>

#define WARN(...)                               \
  {                                             \
    fprintf(stderr, "Warning: " __VA_ARGS__);   \
    fprintf(stderr, "\n");                      \
  }

#define FATAL(...)                                      \
  {                                                     \
    fprintf(stderr, "Fatal error: " __VA_ARGS__);       \
    fprintf(stderr, "\n");                              \
    exit(1);                                            \
  }

#define DBG(...)                                \
  {                                             \
    fprintf(stderr, "Debug: " __VA_ARGS__);     \
    fprintf(stderr, "\n");                      \
  }

#endif /* UTIL_H */
