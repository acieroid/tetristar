newplayer = {}

function newplayer.newplayer(id, command, args)
   -- NEWPLAYER ID NICK
   first_space = string.find(args, " ")
   user_id = tonumber(string.sub(args, 0, first_space))
   nick = string.sub(args, first_space)
   print(tetris.client)
   print(tetris.players)
   tetris.player.add(user_id)
   tetris.player.set_nick(user_id, nick)
   tetris.client.chat_add_line(" * " .. nick .. " is connected\n")
end

tetris.plugin.register("RECV", newplayer.newplayer, "NEWPLAYER")