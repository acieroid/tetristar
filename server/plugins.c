/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "plugins.h"

static int l_send(lua_State *l);
static int l_send_to_all(lua_State *l);
static int l_disconnect(lua_State *l);
static int l_get_password(lua_State *l);

static PluginFunction l_functions[] = {
  { "send", l_send },
  { "send_to_all", l_send_to_all },
  { "disconnect", l_disconnect },
  { "get_password", l_get_password },
  { NULL, NULL }
};

void plugins_init()
{
  /* setup functions */
  tetris_plugin_add_category("server");
  tetris_plugin_add_functions("server", l_functions);
  /* load plugins */
  plugins_load_all();
}

void plugins_deinit()
{
}

void plugins_load_all()
{
  GSList *plugins_to_load;
  plugins_to_load = config_get_list("plugins", NULL);;
  g_slist_foreach(plugins_to_load,
                  (GFunc) tetris_plugin_file_load, NULL);
  g_slist_free_full(plugins_to_load, (GDestroyNotify) g_free);
}

int l_send(lua_State *l)
{
  int id;
  gchar *str = NULL;
  Client *client = NULL;

  luaL_checktype(l, 1, LUA_TNUMBER);
  luaL_checktype(l, 2, LUA_TSTRING);

  id = lua_tonumber(l, 1);
  str = g_strdup(lua_tostring(l, 2));
  client = network_find_client(id);

  if (client != NULL) {
    network_send(client, str);
  } else {
    g_warning("tetris.server.send: Player %d does not exists", id);
  }

  g_free(str);
  return 0;
}

int l_send_to_all(lua_State *l)
{
  gchar *str = NULL;

  luaL_checktype(l, 1, LUA_TSTRING);
  str = g_strdup(lua_tostring(l, 1));
  network_send_to_all(str);

  g_free(str);
  return 0;
}

int l_disconnect(lua_State *l)
{
  int id;
  Client *client = NULL;

  luaL_checktype(l, 1, LUA_TNUMBER);
  id = lua_tonumber(l, 1);
  client = network_find_client(id);

  if (client != NULL) {
    enet_peer_disconnect(client, 0);
  } else {
    g_warning("tetris.server.disconnect: Player %d does not exists", id);
  }

  return 0;
}

int l_get_password(lua_State *l)
{
  char *password = (char *) config_get_string("password", "foo");
  lua_pushstring(l, password);
  free(password);
  return 1;
}
