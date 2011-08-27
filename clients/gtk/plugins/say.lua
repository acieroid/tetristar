say = {}

function say.say(id, command, args)
   -- SAY ID TEXT
   local user_id, text = utils.split(args, " ",
                                     tonumber, utils.identity)
   text = string.sub(args, first_space+1)
   nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text("<" .. nick .. "> " .. text .. "\n")
end

tetris.plugin.register("RECV", say.say, "SAY")