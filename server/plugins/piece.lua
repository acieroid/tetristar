piece = {}

piece.pieces = {
-- The T
   {              {  0, -1 },
      { -1,  0 }, {  0,  0 }, {  1, 1 }
   },
-- The L
   {
                              {  1, -1 },
      { -1,  0 }, {  0,  0 }, {  1,  0 }
   },
-- The J
   {
      { -1,  0 }, {  0,  0 }, {  1,  0 },
                              {  1,  1 },
   },
-- The I
   {
      { -2,  0 }, { -1,  0 }, {  0,  0 }, {  1, 0  }
   },
-- The S
   {
                  {  0, -1 }, {  1, -1 },
      { -1,  0 }, {  0,  0 }
   }
-- The Z
   {
      { -1,  0 }, {  0,  0 },
                  {  0,  1 }, {  1,  1 }
   },
-- The O
   {
      { 0, 0 }, { 1, 0 },
      { 0, 1 }, { 1, 1 }
   }
}

function piece.random_piece()
   return piece.pieces[math.random(table.getn(piece.pieces))]
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
   return { a[0] + b[0], a[1] + b[1] }
end

function piece.copy(p)
   local copy = {}
   for i, cell in pairs(p) do
      copy[i] = {}
      copy[i][0] = cell[0]
      copy[i][1] = cell[1]
      copy[i][2] = cell[2]
   end
   return copy
end

function piece.move(p, direction)
   local inc_pos = piece.position_from_direction(direction)

   local new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][0] = cell[0] + inc_pos[0]
      new_p[i][1] = cell[1] + inc_pos[1]
   end
   return new_p
end

function piece.shift(p, position)
   locan new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][0] = cell[0] + position[0]
      new_p[i][1] = cell[1] + position[1]
   end
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
      return
   end

   local new_p = piece.copy(p)
   for i, cell in pairs(p) do
      new_p[i][0] = cell[0]*rot[0][0] + cell[1]*rot[1][0]
      new_p[i][1] = cell[0]*rot[1][0] + cell[1]*rot[1][1]
   end
   return new_p
end
