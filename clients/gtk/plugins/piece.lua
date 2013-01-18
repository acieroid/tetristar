-- TODO: those functions are taken from the server's plugin. Some of
-- the server's plugin could go in libtetris to avoid copy-pasting
-- stuff like this.
piece = {}

-- Return a (dx, dy) pair corresponding to a given direction
function piece.position_from_direction(direction)
   local inc_x = 0
   local inc_y = 0
   if direction == "LEFT" then
      inc_x = -1
   elseif direction == "RIGHT" then
      inc_x = 1
   elseif direction == "UP" then
      inc_y = -1
   elseif direction == "DOWN" then
      inc_y = 1
   end
   return { inc_x, inc_y }
end

-- Add two position pairs
function piece.add_positions(a, b)
   return { a[1] + b[1], a[2] + b[2] }
end

-- Move a piece in a direction
function piece.move(p, direction)
   local inc_pos = piece.position_from_direction(direction)
   local new_p = {}
   for i, cell in pairs(p) do
      new_p[i] = {cell[1] + inc_pos[1],
                  cell[2] + inc_pos[2],
                  cell[3]}
   end
   return new_p
end

-- Move a piece by a certain distance
function piece.shift(p, position)
   local new_p = {}
   for i, cell in pairs(p) do
      new_p[i] = {cell[1] + position[1],
                  cell[2] + position[2],
                  cell[3]}
   end
   return new_p
end
