
connection = {}

function connection.hello(id, command, args)
   nick = args
   -- a client can only send *one* HELLO
   if not tetris.player.exists(id) then
     if tetris.player.nick_available(nick) then
        -- Send his nick to the player
        tetris.server.send(id, "HELLO " .. id .. " " .. nick)
        -- Send all the players connected to the player
        for i,player_id in pairs(tetris.player.all()) do
           player_nick = tetris.player.get_nick(player_id) 
           tetris.server.send(id, "NEWPLAYER " .. player_id .. " " .. player_nick)

           local state
           if tetris.player.is_playing(player_id) then
              state = "PLAYING"
           else
              state = "NOTPLAYING"
           end
           player_state = tetris.player.get_state(player_id)
           tetris.server.send(id, "STATE " .. player_id .. " " .. state)
        end
        -- Add the player
        tetris.player.add(id)
        tetris.player.set_nick(id, nick)
        -- Tell the other players about this one
        tetris.server.send_to_all("NEWPLAYER " .. id .. " " .. nick)
        tetris.player.set_playing(id, false)
        tetris.server.send_to_all("STATE " .. id .. " NOTPLAYING")
     else
        -- Nick already taken, try appending a '_'
        connection.hello(id, command, args .. "_")
     end
  end
end

function connection.disconnect(id)
   tetris.player.remove(id)
   tetris.server.send_to_all("BYE " .. id)
end

function connection.bye(id, command, args)
   tetris.server.disconnect(id)
end

tetris.plugin.register("RECV", connection.hello, "HELLO")
tetris.plugin.register("DISCONNECT", connection.disconnect)
tetris.plugin.register("RECV", connection.bye, "BYE")
