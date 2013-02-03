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

connection = {}

function connection.hello(id, command, args)
   local nick = args
   local tries = 3 -- try with 3 different nicks before failing
   while tries > 0 do
      -- a client can only send *one* HELLO
      if not tetris.player.exists(id) then
         if tetris.player.nick_available(nick) then
            -- Send his nick to the player
            tetris.server.send(id,
                               string.format("HELLO %d %s", id, nick))
            -- Send all the other players' information to the new player
            for i,player_id in pairs(tetris.player.all()) do
               player_nick = tetris.player.get_nick(player_id) 
               tetris.server.send(id,
                                  string.format("NEWPLAYER %d %s",
                                                player_id, player_nick))

               -- Send the state of other players
               local state
               if tetris.player.is_playing(player_id) then
                  state = "PLAYING"
               else
                  state = "NOTPLAYING"
               end
               tetris.server.send(id,
                                  string.format("STATE %d %s", player_id, state))

               -- Send the field of other players
               local fieldspec = game.to_fieldspec(tetris.matrix.get_cells(player_id))
               tetris.server.send(id,
                                  string.format("FIELD %d %s",
                                                player_id, fieldspec))
            end
            -- Add the player
            tetris.player.add(id)
            tetris.player.set_nick(id, nick)
            -- Tell the other players about this one
            tetris.server.send_to_all(string.format("NEWPLAYER %d %s", id, nick))
            tetris.player.set_playing(id, false)
            tetris.server.send_to_all(string.format("STATE %d NOTPLAYING", id))
            return -- done
         end
         -- Nick already taken, try appending a '_'
         nick = nick .. "_"
         tries = tries - 1
      end
   end
   -- No valid nick, refuse the connection
   tetris.server.send(id, "NICKTAKEN")
   tetris.server.disconnect(id)
   tetris.player.remove(id)
end

function connection.disconnect(id)
   tetris.player.remove(id)
   tetris.server.send_to_all(string.format("BYE %d", id))
end

function connection.bye(id, command, args)
   tetris.server.disconnect(id)
   tetris.player.remove(id)
   -- If it was the last player, stop the game
   if #tetris.player.all() == 0 then
      tetris.game.stop()
   end
end

tetris.plugin.register("RECV", connection.hello, "HELLO")
tetris.plugin.register("DISCONNECT", connection.disconnect)
tetris.plugin.register("RECV", connection.bye, "BYE")
