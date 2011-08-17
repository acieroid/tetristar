im = {}

function im.say(id, command, args)
   server.send_to_all(command .. " " .. id .. " " .. args)
end

server.register("RECV", im.say, "SAY")
