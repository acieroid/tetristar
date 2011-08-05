im = {}

function im.say(id, command, args)
   --print(tetris.players.get_nick(id) .. ": " .. args)
   server.send_to_all(id .. " " .. command .. " " .. args)
end

server.register("RECV", im.say, "SAY")