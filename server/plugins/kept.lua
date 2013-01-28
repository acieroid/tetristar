kept = {}

function kept.keep(id, command, args)
   local kept = tetris.player.get_kept_piece(id)
   tetris.server.send(id,
                      string.format("KEPT %s", game.to_fieldspec(kept)))
   tetris.player.set_kept_piece(tetris.player.get_piece(id))
   if kept == {} then
      field.new_piece(id)
      game.send_piece(id)
      game.send_next_piece(id)
   else
      tetris.player.set_piece(id, kept)
   end
end

tetris.plugin.register("RECV", kept.keep, "KEEP")