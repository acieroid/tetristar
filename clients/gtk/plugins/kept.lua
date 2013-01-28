kept = {}

function kept.kept(id, command, args)
   -- KEPT FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ",
                                          tonumber, utils.parse_fieldspec)
   tetris.player.set_kept_piece(user_id, fieldspec)
   -- tetris.client.context_kept_piece_changed(user_id)
end

tetris.plugin.register("RECV", kept.kept, "KEPT")