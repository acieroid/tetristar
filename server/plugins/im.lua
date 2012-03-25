im = {}

function im.say(id, command, args)
   tetris.server.send_to_all(command .. " " .. id .. " " .. args)
end

function im.servmsg(text)
   -- Send a message from the server to all the clients
   tetris.server.send_to_all("SERVMSG " .. text)
end

tetris.plugin.register("RECV", im.say, "SAY")
