field = {}

function field.check_if_lost(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))

   if not field.is_valid_piece(id, p) then
      tetris.player.set_playing(id, false)
      tetris.server.send_to_all("STATE " .. id .. " NOTPLAYING")
      tetris.server.send_to_all("LOST " .. id)
   end
   -- TODO: check if it's the last player to lose
end

-- Give a new piece to a player
function field.new_piece(id)
   -- TODO: don't hardcode the initial position
   local initial_pos = {4, 0}
   local p = piece.random_piece()
   tetris.player.set_piece(id, p)
   tetris.player.set_piece_position(id, initial_pos)
   field.check_if_lost(id)
end

function field.is_valid_piece(id, p)
   for i, cell in pairs(p) do
      if tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0 then
         return false
      end
   end
   return true
end

function field.can_move(id, direction)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   p = piece.move(p, direction)
   return field.is_valid_piece(id, p)
end

function field.move(id, direction)
   -- move without checking
   local current_pos = tetris.player.get_piece_position(id)
   local new_pos = piece.add_positions(current_pos,
                                       piece.position_from_direction(direction))
   tetris.player.set_piece_position(id, new_pos)
   field.check_if_lost(id)
end

function field.can_rotate(id, direction)
   local p = tetris.player.get_piece(id)
   -- take care of doing the rotation before the translation
   p = piece.rotate(p, direction)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   return field.is_valid_piece(id, p)
end

function field.rotate(id, direction)
   -- rotate without checking
   local p = tetris.player.get_piece(id)
   tetris.player.set_piece(id, piece.rotate(p, direction))
   field.check_if_lost(id)
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
