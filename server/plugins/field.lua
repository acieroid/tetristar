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
   if not field.is_valid_player_piece(id) then
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
   local nlines = 0
   local line_complete = false
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
         nlines = nlines+1
         -- Move every upper line down
         for upper_line = line-1, 0, -1 do
            for column = 0, tetris.matrix.get_width(id)-1 do
               local cell = tetris.matrix.get_uncommited_cell(id, column, upper_line)
               tetris.matrix.set_cell(id, column, upper_line+1, cell)
            end
         end
      end
   end
   field.lines_cleared(id, nlines)
end

-- Called when a player cleared n lines
function field.lines_cleared(id, n)
   -- Add points
   local points = 0
   if n == 1 then
      points = 40
   elseif n == 2 then
      points = 100
   elseif n == 3 then
      points = 300
   elseif n == 4 then
      points = 1200
   else
      -- invalid number of lines cleared
      return
   end

   -- Send lines to other players
   tetris.player.add_points(id, points)
   if n > 1 and n <= 4 then
      for i, player_id in pairs(tetris.player.all()) do
         if player_id ~= id then
            -- Add the lines
            field.add_lines(player_id, n-1)
            -- Send the field
            game.send_field(player_id)
         end
      end
      tetris.server.send_to_all("SERVMSG " .. tetris.player.get_nick(id) .. " sent " .. n-1 .. " lines to everyone")
   end

   -- Statistics
   stats.lines_cleared(id, n)
end

-- Add n lines on the bottom of a player's field
function field.add_lines(id, n)
   for line = 0, n do
      field.add_line(id)
   end
end

-- Add a line on the bottom of a player's field
function field.add_line(id)
   local lost = false
   -- Check if the player will lose when adding a line
   for column = 0, tetris.matrix.get_width(id)-1 do
      local cell = tetris.matrix.get_uncommited_cell(id, column, 0)
      if cell ~= 0 then
         lost = true
      end
   end

   -- Move every line up
   for lower_line = 1, tetris.matrix.get_height(id)-1 do
      for column = 0, tetris.matrix.get_width(id)-1 do
         local cell = tetris.matrix.get_uncommited_cell(id, column, lower_line)
         tetris.matrix.set_cell(id, column, lower_line-1, cell)
      end
   end

   local line = tetris.matrix.get_height(id)-1
   -- Fill the new line with random pieces
   for column = 0, tetris.matrix.get_width(id)-1 do
      local cell = piece.random_cell()
      tetris.matrix.set_cell(id, column, line, cell)
   end
   -- Be sure to have at least one blank cell
   tetris.matrix.set_cell(id, math.random(tetris.matrix.get_width(id)-1), line, 0)

   if lost then
      game.lost(id)
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
   local p_check = piece.shift(p, initial_pos)
   if field.is_valid_piece_uncommited(id, p_check) then
      tetris.player.set_piece(id, p)
      tetris.player.set_piece_position(id, initial_pos)
   else
      -- Can't spawn a new piece, the player loses
      tetris.player.set_piece(id, {})
      game.lost(id)
   end
end

-- Uncommited version of field.is_valid_piece
function field.is_valid_piece_uncommited(id, p)
   for i, cell in pairs(p) do
      if (tetris.matrix.get_uncommited_cell(id, cell[1], cell[2]) ~= 0) then
         return false
      end
   end
   return true
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

   -- change the piece
   field.new_piece(id)
   -- add the current piece to the matrix
   for i, cell in pairs(p) do
      tetris.matrix.set_cell(id, cell[1], cell[2], cell[3])
   end
   -- check if we lost
   field.check_if_lost(id)
   -- clear the lines if necesarry
   field.clear_lines(id)
   -- statistics
   stats.block_dropped(id)
end

-- Add the player piece to the player's matrix
function field.add_player_piece(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   for i, cell in pairs(p) do
      tetris.matrix.set_cell(id, cell[1], cell[2], cell[3])
   end
end

-- Check if a player's piece is well placed
function field.is_valid_player_piece(id)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   return field.is_valid_piece(id, p)
end