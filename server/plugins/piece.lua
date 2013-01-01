piece = {}

-- All the possibles cells
piece.cells = {0, 1, 2, 3, 4, 5, 6, 7}

-- All the possible pieces
piece.pieces = {
-- The T
   {                { 0, -1, 1 },
      { -1, 0, 1 }, { 0,  0, 1 }, { 1, 0, 1 }
   },
-- The L
   {
                                 { 1, -1, 2 },
      { -1, 0, 2 }, { 0, 0, 2 }, { 1,  0, 2 }
   },
-- The J
   {
      { -1, -1, 3 }, { 0, -1, 3 }, { 1, -1, 3 },
                                   { 1,  0, 3 },
   },
-- The I
   {
      { -2, 0, 4 }, { -1, 0, 4 }, { 0, 0, 4 }, { 1, 0, 4 }
   },
-- The S
   {
                    { 0, -1, 5 }, { 1, -1, 5 },
      { -1, 0, 5 }, { 0,  0, 5 }
   },
-- The Z
   {
      { -1, -1, 6 }, { 0, -1, 6 },
                     { 0,  0, 6 }, { 1, 0, 6 }
   },
-- The O
   {
      { 0, -1, 7 }, { 1, -1, 7 },
      { 0,  0, 7 }, { 1,  0, 7 }
   }
}

-- Initialize the random state
math.randomseed(os.time())

-- Return a random piece
function piece.random_piece()
   return piece.pieces[math.random(#piece.pieces)]
end

-- Return a random cell
function piece.random_cell()
   return piece.cells[math.random(#piece.cells)]
end

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

-- Rotate a piece in a given direction
function piece.rotate(p, direction)
   -- rotation matrix when theta = 90 degrees
   local rot_right = { { 0, -1 }, { 1, 0 } }
   -- rotation matrix when theta = - 90 degrees
   local rot_left = { { 0, 1 }, { -1, 0 } }
   local rot = {}
   if direction == "RIGHT" then
      rot = rot_right
   elseif direction == "LEFT" then
      rot = rot_left
   else
      return p
   end

   local new_p = {}
   for i, cell in pairs(p) do
      new_p[i] = {cell[1]*rot[1][1] + cell[2]*rot[1][2],
                  cell[1]*rot[2][1] + cell[2]*rot[2][2],
                  cell[3]}
   end
   return new_p
end
