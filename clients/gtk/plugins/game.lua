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

game = {}

function game.state(id, command, args)
   -- STATE ID STATE
   local user_id, state = utils.split(args, " ", tonumber, utils.identity)
   local playing
   if state == "PLAYING" then
      playing = true
   else
      playing = false
   end
   tetris.player.set_playing(user_id, playing)
   tetris.client.context_field_changed(user_id, {}) -- refresh the drawing
end

function game.won(id, command, args)
   -- WON ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("good",
      string.format(" * %s has won\n", nick))
end

function game.lost(id, command, args)
   -- LOST ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("bad",
      string.format(" * %s has lost\n", nick))
end

tetris.plugin.register("RECV", game.state, "STATE")
tetris.plugin.register("RECV", game.won, "WON")
tetris.plugin.register("RECV", game.lost, "LOST")