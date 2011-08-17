#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <assert.h>

#include <glib.h>

typedef struct Command {
  gchar *command;
  int n_args;
  gchar **args;
  gchar *args_str;
} Command;

Command *command_new(const gchar *command_str, const gchar *args_str);
void command_free(Command *command);

#endif /* COMMAND_H */
