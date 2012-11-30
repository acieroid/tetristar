field = {}

-- Check if a player wins (ie. it is the last player playing) and send
-- commands
function field.check_who_wins()
   local last_playing = nil

   -- If we have one player, the game stops when he loses
   if tetris.player.number() == 1 then
      id = tetris.player.all()[1]
      if not tetris.player.is_playing(id) then
         game.stop()
      end
   elseif tetris.player.number() > 1 then
      for i, id in pairs(tetris.player.all()) do
         if tetris.player.is_playing(id) then
            if last_playing == nil then
               last_playing = id
            else
               -- there is at least two playing players
               last_playing = nil
               break
            end
         end
      end

      -- whe have a winner
      if last_playing ~= nil then
         game.win(last_playing)
         game.stop()
      end
   end
end

-- Check if the player ID has lost and send commands to inform clients
-- if that's the case
function field.check_if_lost(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))

   if not field.is_valid_piece(id, p) then
      game.lost(id)
   end
end

-- Print the field of a player
function field.print(id)
   print("Player " .. id .. " field:")
   local cell
   for line = 0, tetris.matrix.get_height(id)-1 do
      for column = 0, tetris.matrix.get_width(id)-1 do
         cell = tetris.matrix.get_cell(id, column, line)
         if cell == 0 then
            io.write(" ")
         else
            io.write(cell)
         end
      end
      io.write("\n")
   end
end

-- Look if there are full lines and if so, clear them
function field.clear_lines(id)
   local line_complete
   for line = 0, tetris.matrix.get_height(id)-1 do
      -- Find a complete line
      line_complete = true
      for column = 0, tetris.matrix.get_width(id)-1 do
         if tetris.matrix.get_uncommited_cell(id, column, line) == 0 then
            line_complete = false
            break
         end
      end

      if line_complete then
         -- Move every upper line down
         for upper_line = line-1, 0, -1 do
            for column = 0, tetris.matrix.get_width(id)-1 do
               local cell = tetris.matrix.get_uncommited_cell(id, column, upper_line)
               tetris.matrix.set_cell(id, column, upper_line+1, cell)
            end
         end
      end
   end
end

-- Clean the entire field of a player
function field.clean(id)
   local EMPTY = 0 -- TODO: get this value from libtetris
   for line = 0, tetris.matrix.get_height(id)-1 do
      for column = 0, tetris.matrix.get_width(id)-1 do
         tetris.matrix.set_cell(id, column, line, EMPTY)
      end
   end
end

-- Give a new piece to a player
function field.new_piece(id)
   -- TODO: don't hardcode the initial position
   local initial_pos = {4, 0}
   local p = piece.random_piece()
   tetris.player.set_piece(id, p)
   tetris.player.set_piece_position(id, initial_pos)
end

function field.is_valid_piece(id, p)
   for i, cell in pairs(p) do
      if (tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0) then
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
end

function field.drop(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   -- add the current piece to the matrix
   for i, cell in pairs(p) do
      tetris.matrix.set_cell(id, cell[1], cell[2], cell[3])
   end
   -- if the piece was not valid, we lost
   if field.is_valid_piece(id, p) then
      -- change the piece
      field.new_piece(id)
   else
      game.lost(id)
   end
   -- check if we lost
   field.check_if_lost(id)
   -- clear the lines if necesarry
   field.clear_lines(id)
end
