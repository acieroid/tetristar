
connection = {}

function connection.hello(id, command, args)
   nick = args
   -- a client can only send *one* HELLO
   if not tetris.players.exists(id) then
     if tetris.players.nick_available(nick) then
        -- Send his nick to the player
        server.send(id, "HELLO " .. id .. " " .. nick)
        -- Send all the players connected to the player
        for i,player_id in pairs(tetris.players.all()) do
           player_nick = tetris.players.get_nick(player_id) 
           server.send(id, "NEWPLAYER " .. player_id .. " " .. player_nick)
        end
        -- Add the player
        tetris.players.add(id)
        tetris.players.set_nick(id, nick)
        -- Tell the other players about this one
        server.send_to_all("NEWPLAYER " .. id .. " " .. nick)
     else
        -- Nick already taken, try appending a '_'
        connection.hello(id, command, args .. "_")
     end
  end
end

function connection.disconnect(id)
   tetris.players.remove(id)
   server.send_to_all("BYE " .. id)
end

server.register("DISCONNECT", connection.disconnect)
server.register("RECV", connection.hello, "HELLO")
