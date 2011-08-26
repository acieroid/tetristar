bye = {}

function bye.bye(id, command, args)
   -- BYE ID
   user_id = tonumber(args)
   nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text(" * " .. nick .. " has left\n")
   tetris.client.context_remove_player(user_id)
   tetris.player.remove(user_id)
end

tetris.plugin.register("RECV", bye.bye, "BYE")