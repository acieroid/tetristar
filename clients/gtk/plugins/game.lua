game = {}

function game.state(id, command, args)
   -- STATE ID STATE
   local user_id, state = utils.split(args, " ", tonumber, utils.identity)
   local playing
   if state == "PLAYING" then
      playing = true
   else
      playing = false
   end
   tetris.player.set_playing(user_id, playing)
   tetris.client.context_field_changed(user_id, {}) -- refresh the drawing
end

function game.won(id, command, args)
   -- WON ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("good",
      string.format(" * %s has won\n", nick))
end

function game.lost(id, command, args)
   -- LOST ID
   local user_id = tonumber(args)
   local nick = tetris.player.get_nick(user_id)
   tetris.client.chat_add_colored_text("bad",
      string.format(" * %s has lost\n", nick))
end

tetris.plugin.register("RECV", game.state, "STATE")
tetris.plugin.register("RECV", game.won, "WON")
tetris.plugin.register("RECV", game.lost, "LOST")