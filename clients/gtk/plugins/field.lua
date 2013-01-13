field = {}

function field.field(id, command, args)
   -- FIELD ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)
   for i, cell in pairs(fieldspec) do
      tetris.matrix.set_cell(user_id, cell[1], cell[2], cell[3])
   end
   tetris.matrix.commit(user_id)
   tetris.client.context_field_changed(user_id)
end

function field.piece(id, command, args)
   -- PIECE ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)
   tetris.player.set_piece(user_id, fieldspec)
   tetris.client.context_field_changed(user_id)
end

function field.nextpiece(id, command, args)
  -- NEXTPIECE ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ",
                                          tonumber, utils.parse_fieldspec)
   tetris.player.set_next_piece(user_id, fieldspec)
   tetris.client.context_next_piece_changed(user_id)
end

function field.piecepos(id, command, args)
   -- PIECEPOS ID X,Y
   local user_id, rest = utils.split(args, " ",
                                     tonumber, utils.identity)
   local x, y = utils.split(rest, ",",
                            tonumber, tonumber)
   tetris.player.set_piece_position(user_id, {x, y})
   tetris.client.context_field_changed(user_id)
end

tetris.plugin.register("RECV", field.field, "FIELD")
tetris.plugin.register("RECV", field.piece, "PIECE")
tetris.plugin.register("RECV", field.nextpiece, "NEXTPIECE")
tetris.plugin.register("RECV", field.piecepos, "PIECEPOS")
