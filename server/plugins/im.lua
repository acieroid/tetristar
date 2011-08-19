im = {}

function im.say(id, command, args)
   tetris.server.send_to_all(command .. " " .. id .. " " .. args)
end

tetris.plugin.register("RECV", im.say, "SAY")
