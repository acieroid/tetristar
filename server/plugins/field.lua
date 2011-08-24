field = {}

function field.is_valid_piece(id, p)
   for i, cell in pairs(p) do
      if tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0 then
         -- already set cell, piece not valid
         return false
      end
   end
   return true
end

function field.can_move(id, direction)
   print("field.can_move: " .. id .. " " .. direction)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   p = piece.move(p, direction)
   return field.is_valid_piece(id, p)
end

function field.move(id, direction)
   print("field.move: " .. id .. " " .. direction)
   -- move without checking
   local current_pos = tetris.player.get_piece_position(id)
   local new_pos = piece.add_positions(current_pos,
                                       piece.position_from_direction(direction))
   tetris.player.set_piece_position(id, new_pos)
end

function field.can_rotate(id, direction)
   local p = tetris.player.get_piece(id)
   -- take care of doing the rotation before the translation
   p = piece.rotate(p, direction)
   p = piece.shift_piece(p, tetris_player_get_piece_position(id))
   return field.is_valid_piece(id, p)
end

function field.rotate(id, direction)
   -- rotate without checking
   local p = tetris.player.get_piece(id)
   tetris.player.set_piece(id, piece.rotate(p, direction))
end

function field.drop(id, direction)
   local p = tetris.player.get_piece(id)
   -- add the current piece to the matrix
   for i, cell in pairs(p) do
      tetris.matrix.set_cell(id, cell[1], cell[2], cell[3])
   end
   -- change the piece
   tetris.player.set_piece(id, piece.random_piece())
end