game = {}

-- Send the field modifications of a player's field
function game.send_field(id)
   -- Get the modifications
   fieldspec = ""
   for i, cell in pairs(tetris.matrix.diff(id)) do
      x = cell[1]
      y = cell[2]
      type = cell[3]
      fieldspec = fieldspec .. ":" .. x .. "," .. y .. "," .. type
   end
   fieldspec = string.sub(fieldspec, 2) -- drop the first colon
   -- Send the modifications
   tetris.server.send_to_all("FIELD " .. id .. " " .. fieldspec)
   -- Apply the modifications
   tetris.matrix.commit(id)
end
   
function game.start(id, command, args)
   if tetris.player.is_admin(id) then
      tetris.game.start()
      tetris.server.send_to_all("START")
      tetris.player.set_piece(id, piece.random_piece())
   end
end

function game.move(id, command, args)
   direction = args
   if not (direction == "LEFT" or direction == "RIGHT" 
           or direction == "DOWN") then
      -- Unknown direction, we don't do anything
      return
   end

   if field.can_move(id, direction) then
      -- If the player can move the piece... well it moves it
      field.move(id, direction)
   elseif direction == "DOWN" then
      -- If the players move it down and he can't, the piece is
      -- dropped and the player get a new piece
      field.drop(id)
      piece.new_piece(id)
   end

   -- Finally send the field modifications
   game.send_field(id)
end

function game.rotate(id, command, args)
   direction = args
   if not (direction == "LEFT" or direction == "RIGHT") then
      -- Unknown direction
      return
   end

   if field.can_rotate(id, direction) then
      field.rotate(id)
   end

   game.send_field(id)
end

function game.drop(id, command, args)
   -- Drop the piece until it touches another piece, but *is still
   -- controllable*
   while field.can_move(id, "DOWN") do
      field.move(id, "DOWN")
   end

   game.send_field(id)
end

function game.update()
   for i, id in pairs(tetris.player.all()) do
      game.move(id, "MOVE", "DOWN")
   end
end

tetris.plugin.register("RECV", game.start, "START")
tetris.plugin.register("RECV", game.move, "MOVE")
tetris.plugin.register("RECV", game.move, "ROTATE")
tetris.plugin.register("RECV", game.drop, "DROP")
tetris.plugin.register("TIMEOUT", game.update, 1)