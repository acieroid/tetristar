chat = {}

function chat.newplayer(id, command, args)
   -- NEWPLAYER ID NICK
   local user_id, nick = utils.split(args, " ", tonumber, utils.identity)
   tetris.player.add(user_id)
   tetris.player.set_nick(user_id, nick)
   tetris.client.chat_add_text(" * " .. nick .. " is connected\n")
   tetris.client.context_add_player(user_id)
end

function chat.admin(id, command, args)
   -- ADMIN ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text(" * " .. nick .. " is now an administrator\n")
   tetris.player.set_admin(user_id, true)
end

function chat.start(id, command, args)
   tetris.client.chat_add_text(" ** The game has started\n")
end

function chat.stop(id, command, args)
   tetris.client.chat_add_text(" ** End of the game\n")
end

function chat.say(id, command, args)
   -- SAY ID TEXT
   local user_id, text = utils.split(args, " ",
                                     tonumber, utils.identity)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text("<" .. nick .. "> " .. text .. "\n")
end

function chat.servmsg(id, command, args)
   -- SERVMSG TEXT
   tetris.client.chat_add_text(">>> server message: " .. args .. "\n")
end

function chat.bye(id, command, args)
   -- BYE ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_text(" * " .. nick .. " has left\n")
   tetris.client.context_remove_player(user_id)
   tetris.player.remove(user_id)
end

function chat.nicktaken(id, command, args)
   -- NICKTAKEN
   -- TODO: the disconnect event seems to be caught before the NICKTAKEN message
   tetris.client.display_error("Nick already taken")
   tetris.client.disconnect()
end   

tetris.plugin.register("RECV", chat.newplayer, "NEWPLAYER")
tetris.plugin.register("RECV", chat.say, "SAY")
tetris.plugin.register("RECV", chat.start, "START")
tetris.plugin.register("RECV", chat.stop, "STOP")
tetris.plugin.register("RECV", chat.admin, "ADMIN")
tetris.plugin.register("RECV", chat.bye, "BYE")
tetris.plugin.register("RECV", chat.servmsg, "SERVMSG")
tetris.plugin.register("RECV", chat.nicktaken, "NICKTAKEN")
