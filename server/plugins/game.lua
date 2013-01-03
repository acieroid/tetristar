game = {}

-- Table that contains the timer for each player. When the timer is
-- over a certain value, the player's piece will move down.
game.timers = {}

-- Convert a field to a fieldspec
function game.to_fieldspec(field)
   local fieldspec = ""
   for i, cell in pairs(field) do
      local x = cell[1]
      local y = cell[2]
      local type = cell[3]
      fieldspec = fieldspec .. ":" .. x .. "," .. y .. "," .. type
   end
   fieldspec = string.sub(fieldspec, 2) -- drop the first colon
   return fieldspec
end

-- Send the field modifications of a player's field
function game.send_field(id)
   local diff = tetris.matrix.diff(id)
   if #diff == 0 then
      -- no modification on the field, so we don't send anything
      return
   end
 
   -- Get the modifications
   local fieldspec = game.to_fieldspec(diff)
   -- Send the modifications
   tetris.server.send_to_all("FIELD " .. id .. " " .. fieldspec)
   -- Apply the modifications
   tetris.matrix.commit(id)
   -- Reset the timer of this player
   game.reset_timer(id)
end

-- Send the actual piece of a player
function game.send_piece(id)
   local p = tetris.player.get_piece(id)
   local fieldspec = game.to_fieldspec(p)
   -- TODO do not send the PIECE command if the piece hasn't changed
   tetris.server.send_to_all("PIECE " .. id .. " " .. fieldspec)

   local pos = tetris.player.get_piece_position(id)
   tetris.server.send_to_all("PIECEPOS " .. id .. " " .. 
                             pos[1] .. "," .. pos[2])
   p = tetris.player.get_next_piece(id)
   fieldspec = game.to_fieldspec(p)
   tetris.server.send_to_all("NEXTPIECE " .. id .. " " .. fieldspec)
end

function game.start(id, command, args)
   if tetris.player.is_admin(id) and not tetris.game.is_started() then
      -- each player is now playing and gets a clean field and a new piece
      for i, player_id in pairs(tetris.player.all()) do
         tetris.player.set_playing(player_id, true)
         tetris.server.send_to_all("STATE " .. player_id .. " PLAYING")
         field.clean(player_id)
         game.send_field(player_id)
         field.new_piece(player_id)
         game.send_piece(player_id)
         stats.player_started(player_id)
      end
      -- start the game
      tetris.game.start()
      tetris.server.send_to_all("START")
   end
end

-- Called when a STOP message is received
function game.adminstop(id, command, args)
   if tetris.player.is_admin(id) and tetris.game.is_started() then
      game.stop()
   end
end

-- Stop the game
function game.stop()
   tetris.game.stop()
   tetris.server.send_to_all("STOP")
   for i, player_id in pairs(tetris.player.all()) do
      if tetris.player.is_playing(player_id) then
         stats.player_finished(player_id)
         tetris.player.set_playing(player_id, false)
         tetris.server.send_to_all("STATE " .. player_id .. " NOTPLAYING")
      end
   end
   stats.send()
end

function game.move(id, command, args)
   if tetris.player.is_playing(id) then
      local direction = args
      if not (direction == "LEFT" or direction == "RIGHT" 
              or direction == "DOWN") then
         -- Unknown direction, we don't do anything
         return
      end

      if field.can_move(id, direction) then
         -- If the player can move the piece... well it moves it
         field.move(id, direction)
         -- Reset the timer only if the piece is moved down
         if direction == "DOWN" then
            game.reset_timer(id)
         end
      elseif direction == "DOWN" then
         -- If the players move it down and he can't, the piece is
         -- dropped, and field.drop gives the player a new piece
         field.drop(id)
      end

      if tetris.player.is_playing(id) then
         -- Don't send a new piece if lost
         game.send_piece(id)
      end
      -- Finally send the field modifications
      game.send_field(id)
   end
end

function game.rotate(id, command, args)
   if tetris.player.is_playing(id) then
      local direction = args
      if not (direction == "LEFT" or direction == "RIGHT") then
         -- Unknown direction
         return
      end

      if field.can_rotate(id, direction) then
         field.rotate(id, direction)
      end

      if tetris.player.is_playing(id) then
         game.send_piece(id)
      end
      game.send_field(id)
   end
end

function game.drop(id, command, args)
   if tetris.player.is_playing(id) then
      -- Drop the piece until it touches another piece
      while field.can_move(id, "DOWN") do
         field.move(id, "DOWN")
      end
      field.drop(id)

      if tetris.player.is_playing(id) then
         game.send_piece(id)
      end
      game.send_field(id)
   end
end

-- Reset the timer for a player
function game.reset_timer(id)
   game.timers[id] = 0
end

-- Update the timer for a player
function game.update_timer(id, delta)
   prev = game.timers[id]
   if prev then
      game.timers[id] = prev + delta
   else
      game.timers[id] = delta
   end
end

-- Check if the timer of a player timed out
function game.timed_out(id)
   if game.timers[id] then
      return game.timers[id] > 1000000 -- 1s
   else
      return false
   end
end

-- Move the piece of every player down
function game.update(delta)
   if tetris.game.is_started() then
      for i, id in pairs(tetris.player.all()) do
         game.update_timer(id, delta)
         if tetris.player.is_playing(id) and game.timed_out(id) then
            game.move(id, "MOVE", "DOWN")
            game.reset_timer(id)
         end
      end
   end
end

-- A player just lost
function game.lost(id)
   if tetris.player.is_playing(id) then
      stats.player_finished(id)
      tetris.player.set_playing(id, false)
      tetris.server.send_to_all("STATE " .. id .. " NOTPLAYING")
      tetris.server.send_to_all("LOST " .. id)
      field.check_who_wins()
   end
end

-- We have a winner
function game.win(id)
   if tetris.player.is_playing(id) then
      stats.player_finished(id)
      tetris.player.set_playing(id, false)
      tetris.server.send_to_all("STATE " .. id .. " NOTPLAYING")
      tetris.server.send_to_all("WON " .. id)
      winlist.add(id)
   end
end

tetris.plugin.register("RECV", game.start, "START")
tetris.plugin.register("RECV", game.stop, "STOP")
tetris.plugin.register("RECV", game.move, "MOVE")
tetris.plugin.register("RECV", game.rotate, "ROTATE")
tetris.plugin.register("RECV", game.drop, "DROP")
tetris.plugin.register("TIMEOUT", game.update, 100000) -- 0.1s