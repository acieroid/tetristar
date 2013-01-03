connection = {}

function connection.hello(id, command, args)
   local nick = args
   local tries = 3 -- try with 3 different nicks before failing
   while tries > 0 do
      -- a client can only send *one* HELLO
      if not tetris.player.exists(id) then
         if tetris.player.nick_available(nick) then
            -- Send his nick to the player
            tetris.server.send(id, "HELLO " .. id .. " " .. nick)
            -- Send all the other players' information to the new player
            for i,player_id in pairs(tetris.player.all()) do
               player_nick = tetris.player.get_nick(player_id) 
               tetris.server.send(id, "NEWPLAYER " .. player_id .. " " .. player_nick)

               -- Send the state of other players
               local state
               if tetris.player.is_playing(player_id) then
                  state = "PLAYING"
               else
                  state = "NOTPLAYING"
               end
               tetris.server.send(id, "STATE " .. player_id .. " " .. state)

               -- Send the field of other players
               local fieldspec = game.to_fieldspec(tetris.matrix.get_cells(player_id))
               tetris.server.send(id, "FIELD " .. player_id .. " " .. fieldspec)
            end
            -- Add the player
            tetris.player.add(id)
            tetris.player.set_nick(id, nick)
            -- Tell the other players about this one
            tetris.server.send_to_all("NEWPLAYER " .. id .. " " .. nick)
            tetris.player.set_playing(id, false)
            tetris.server.send_to_all("STATE " .. id .. " NOTPLAYING")
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
   tetris.server.send_to_all("BYE " .. id)
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
