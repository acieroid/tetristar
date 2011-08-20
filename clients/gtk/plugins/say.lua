say = {}

function say.say(id, command, args)
   -- SAY ID TEXT
   first_space = string.find(args, " ")
   user_id = tonumber(string.sub(args, 0, first_space))
   text = string.sub(args, first_space+1)
   nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text("<" .. nick .. "> " .. text .. "\n")
end

tetris.plugin.register("RECV", say.say, "SAY")