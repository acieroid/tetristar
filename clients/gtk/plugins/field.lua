field = {}

function field.field(id, command, args)
   -- FIELD ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)
   for i, cell in pairs(fieldspec) do
      tetris.matrix.set_cell(user_id, cell[1], cell[2], cell[3])
   end
   tetris.matrix.commit(user_id)
   tetris.client.context_field_changed(user_id, fieldspec)
   field.update_shadow(user_id)
end

function field.piece(id, command, args)
   -- PIECE ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)

   tetris.client.context_field_changed(user_id, {})

   -- Set new piece
   tetris.player.set_piece(user_id, fieldspec)
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

   -- Set the new piece
   tetris.player.set_piece_position(user_id, {x, y})
   field.update_shadow(user_id, p)
end

function field.is_player_piece_on_cell(id, cell)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   for j, pcell in pairs(tetris.player.get_piece(id)) do
      if (pcell[1] == cell[1] and pcell[2] == cell[2]) then
         return true
      end
   end
   return false
end

function field.is_valid(id, p)
   for i, cell in pairs(p) do
      if (tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0 and
          -- acceptable if it is on the player's piece
           not field.is_player_piece_on_cell(id, cell)) then
         return false
      end
   end
   return true
end

-- Update the displayed shadow of a player
-- TODO: only do this for the client's player
function field.update_shadow(id)
   local p = tetris.player.get_piece(id)
   if #p ~= 0 then
      p = piece.shift(p, tetris.player.get_piece_position(id))
      if field.is_valid(id, p) then
         while field.is_valid(id, p) do
            p = piece.move(p, "DOWN")
         end
         p = piece.move(p, "UP")
         tetris.client.context_set_shadow(id, p)
      else
         -- The player has lost, no shadow
         tetris.client.context_set_shadow(id, {})
      end
   end
end

tetris.plugin.register("RECV", field.field, "FIELD")
tetris.plugin.register("RECV", field.piece, "PIECE")
tetris.plugin.register("RECV", field.nextpiece, "NEXTPIECE")
tetris.plugin.register("RECV", field.piecepos, "PIECEPOS")
