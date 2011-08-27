newplayer = {}

function newplayer.newplayer(id, command, args)
   -- NEWPLAYER ID NICK
   local user_id, nick = utils.split(args, " ", tonumber, utils.identity)
   tetris.player.add(user_id)
   tetris.player.set_nick(user_id, nick)
   tetris.client.chat_add_text(" * " .. nick .. " is connected\n")
   tetris.client.context_add_player(user_id)
end

tetris.plugin.register("RECV", newplayer.newplayer, "NEWPLAYER")