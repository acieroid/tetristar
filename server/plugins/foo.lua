
foo = {}

function tetris.nick_available(nick)
   return true
end

function foo.connect(id)
   print("Player " .. id .. " connected");
end

function foo.hello(id, command, args)
   nick = args
   print("Player " .. id .. " wants the nick " .. nick)
   if tetris.nick_available(nick) then
      print("Ok, player added")
      tetris.players.add(id)
      tetris.players.set_nick(id, nick)
      server.send(id, "HELLO " .. id .. " " .. nick)
      server.send_to_all("NEWPLAYER " .. id .. " " .. nick)
   else
      print("Nick already taken, adding a _")
      foo.hello(id, command, args .. "_")
   end
end

function foo.disconnect(id)
   print("Player " .. id .. " disconnected")
   tetris.players.remove(id)
end

server.register("CONNECT", foo.connect)
server.register("DISCONNECT", foo.disconnect)
server.register("RECV", foo.hello, "HELLO")