-- Copyright (c) 2013, Quentin Stievenart
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
--
-- Redistributions of source code must retain the above copyright
-- notice, this list of conditions and the following disclaimer.
--
-- Redistributions in binary form must reproduce the above copyright
-- notice, this list of conditions and the following disclaimer in the
-- documentation and/or other materials provided with the
-- distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
-- "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
-- COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
-- INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
-- HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
-- OF THE POSSIBILITY OF SUCH DAMAGE.

chat = {}

function chat.newplayer(id, command, args)
   -- NEWPLAYER ID NICK
   local user_id, nick = utils.split(args, " ", tonumber, utils.identity)
   tetris.player.add(user_id)
   tetris.player.set_nick(user_id, nick)
   tetris.client.chat_add_colored_text("info",
      string.format(" * %s is connected\n", nick))
   tetris.client.context_add_player(user_id)
end

function chat.admin(id, command, args)
   -- ADMIN ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("info",
      string.format(" * %s is now an administrator\n", nick))
   tetris.player.set_admin(user_id, true)
end

function chat.start(id, command, args)
   tetris.client.chat_add_colored_text("info", " ** The game has started\n")
end

function chat.stop(id, command, args)
   tetris.client.chat_add_colored_text("info", " ** End of the game\n")
end

function chat.say(id, command, args)
   -- SAY ID TEXT
   local user_id, text = utils.split(args, " ",
                                     tonumber, utils.identity)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("normal",
      string.format("<%s> %s\n", nick, text))
end

function chat.servmsg(id, command, args)
   -- SERVMSG TEXT
   tetris.client.chat_add_colored_text("info",
      string.format(">>> server message: %s\n", args))
end

function chat.bye(id, command, args)
   -- BYE ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("info",
      string.format(" * %s has left\n", nick))
   tetris.client.context_remove_player(user_id)
   tetris.player.remove(user_id)
end

function chat.nicktaken(id, command, args)
   -- NICKTAKEN
   -- TODO: the disconnect event seems to be caught before the NICKTAKEN message
   tetris.client.display_error("Nick already taken or invalid")
   tetris.client.disconnect()
end

tetris.plugin.register("RECV", chat.newplayer, "NEWPLAYER")
tetris.plugin.register("RECV", chat.say, "SAY")
tetris.plugin.register("RECV", chat.start, "START")
tetris.plugin.register("RECV", chat.stop, "STOP")
tetris.plugin.register("RECV", chat.admin, "ADMIN")
tetris.plugin.register("RECV", chat.bye, "BYE")
tetris.plugin.register("RECV", chat.servmsg, "SERVMSG")
tetris.plugin.register("RECV", chat.nicktaken, "NICKTAKEN")
