
connection = {}

function connection.hello(id, command, args)
   nick = args
   -- a client can only send *one* HELLO
   if not tetris.players.exists(id) then
     if tetris.players.nick_available(nick) then
        tetris.players.add(id)
        tetris.players.set_nick(id, nick)
        server.send(id, "HELLO " .. id .. " " .. nick)
        server.send_to_all("NEWPLAYER " .. id .. " " .. nick)
     else
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
