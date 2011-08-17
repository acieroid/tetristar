#include "command.h"

Command *command_new(const gchar *command_str, const gchar *args_str)
{
  int i, last_space, arg;
  Command *command = malloc(sizeof(*command));
  assert(command != NULL);

  command->command = g_strdup(command_str);

  command->n_args = 0;
  for (i = 0; args_str[i] != '\0'; i++)
    if (args_str[i] == ' ')
      command->n_args++;
  command->n_args++;

  command->args_str = g_strdup(args_str);
  command->args = malloc(command->n_args*sizeof(*(command->args)));
  last_space = -1;
  arg = 0;
  for (i = 0; ; i++) {
    if (args_str[i] == ' ' || args_str[i] == '\0') {
      command->args[arg] = g_strndup(args_str+last_space+1, i-last_space);
      
      if (args_str[i] == '\0')
        break;
      else
        command->args[arg][i-last_space-1] = '\0';

      arg++;
      last_space = i;
    }
  }
  /* TODO: check that there are enough arguments here */
  return command;
}

void command_free(Command *command)
{
  int i;
  assert(command != NULL);
  if (command->command != NULL)
    g_free(command->command);
  if (command->args != NULL) {
    for (i = 0; i < command->n_args; i++)
      g_free(command->args[i]);
    g_free(command->args);
  }
}
