piece = {}

piece.pieces = {
-- The T
   {                { 0, -1, 1 },
      { -1, 0, 1 }, { 0,  0, 1 }, { 1, 1, 1 }
   },
-- The L
   {
                                 { 1, -1, 2 },
      { -1, 0, 2 }, { 0, 0, 2 }, { 1,  0, 2 }
   },
-- The J
   {
      { -1, 0, 3 }, { 0, 0, 3 }, { 1, 0, 3 },
                                 { 1, 1, 3 },
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
      { -1, 0, 6 }, { 0, 0, 6 },
                    { 0, 1, 6 }, { 1, 1, 6 }
   },
-- The O
   {
      { 0, 0, 7 }, { 1, 0, 7 },
      { 0, 1, 7 }, { 1, 1, 7 }
   }
}

function piece.random_piece()
   return piece.pieces[math.random(#piece.pieces)]
end

function piece.position_from_direction()
   local inc_x = 0
   local inc_y = 0
   if direction == "LEFT" then
      inc_x = 1
   elseif direction == "RIGHT" then
      inc_x = -1
   elseif direction == "UP" then
      inc_y = -1
   elseif direction == "DOWN" then
      inc_y = 1
   end
   return { inc_x, inc_y }
end

function piece.add_positions(a, b)
   return { a[1] + b[1], a[2] + b[2] }
end

function piece.copy(p)
   local copy = {}
   for i, cell in pairs(p) do
      copy[i] = {}
      copy[i][1] = cell[1]
      copy[i][2] = cell[2]
      copy[i][3] = cell[3]
   end
   return copy
end

function piece.move(p, direction)
   local inc_pos = piece.position_from_direction(direction)

   local new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][1] = cell[1] + inc_pos[1]
      new_p[i][2] = cell[2] + inc_pos[2]
   end
   return new_p
end

function piece.shift(p, position)
   local new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][1] = cell[1] + position[1]
      new_p[i][2] = cell[2] + position[2]
   end
   return new_p
end

function piece.rotate(p, direction)
   -- rotation matrix when theta = 90 degrees
   local rot_right = { { 0 -1 }, { 1, 0 } }
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

   local new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][1] = cell[1]*rot[1][1] + cell[2]*rot[2][1]
      new_p[i][2] = cell[1]*rot[2][1] + cell[2]*rot[2][2]
   end
   return new_p
end
