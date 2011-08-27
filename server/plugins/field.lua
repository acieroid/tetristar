field = {}

-- Give a new piece to a player
function field.new_piece(id)
   tetris.player.set_piece(id, piece.random_piece())
   -- TODO: don't hardcode the initial position
   tetris.player.set_piece_position(id, {4, 0})
end
   
function field.is_valid_piece(id, p)
   --print("field.is_valid_piece")
   for i, cell in pairs(p) do
      -- print(i .. ": " .. cell[1] .. ", " .. cell[2] .. " -> " ..
      --tetris.matrix.get_cell(id, cell[1], cell[2]))
      if tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0 then
         return false
      end
   end
   return true
end

function field.can_move(id, direction)
   local p = tetris.player.get_piece(id)
   --print("Pos: " .. tetris.player.get_piece_position(id)[2])
   p = piece.shift(p, tetris.player.get_piece_position(id))
   p = piece.move(p, direction)
   --print("Piece y: " .. p[1][2])
   return field.is_valid_piece(id, p)
end

function field.move(id, direction)
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

function field.drop(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   -- add the current piece to the matrix
   for i, cell in pairs(p) do
      tetris.matrix.set_cell(id, cell[1], cell[2], cell[3])
   end
   -- change the piece
   field.new_piece(id)
end

