print("Loading foo.lua");

foo = {}

function foo.connect(id)
  print("Player " .. id .. " connected");
end

function foo.disconnect(id)
  print("Player " .. id .. " disconnected")
end

register("CONNECT", foo.connect)
register("DISCONNECT", foo.disconnect)
