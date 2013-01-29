kept = {}

-- Keep the current piece, swapping it with the previously kept piece
-- (or with a new one if no piece has been kept previously)
function kept.keep(id, command, args)
   local piece = tetris.player.get_kept_piece(id)

   tetris.server.send(id,
                      string.format("KEPT %d %s", id, 
                                    game.to_fieldspec(tetris.player.get_piece(id))))
   tetris.player.set_kept_piece(id, tetris.player.get_piece(id))
   if #piece == 0 then
      field.new_piece(id)
   else
      tetris.player.set_piece(id, piece)
   end
   game.send_piece(id)
   game.send_next_piece(id)
end

tetris.plugin.register("RECV", kept.keep, "KEEP")