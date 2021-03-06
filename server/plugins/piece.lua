-- Copyright (c) 2013, Quentin Stievenart
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
--
-- Redistributions of source code must retain the above copyright
-- notice, this list of conditions and the following disclaimer.
--
-- Redistributions in binary form must reproduce the above copyright
-- notice, this list of conditions and the following disclaimer in the
-- documentation and/or other materials provided with the
-- distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
-- "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
-- COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
-- INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
-- HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
-- OF THE POSSIBILITY OF SUCH DAMAGE.

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

   -- Empty piece
   if #p == 0 then
      return p
   end

   -- Special case rotations
   if p[1][3] == 7 then
      -- The O don't rotate
      return p
   elseif ((p[1][3] == 4 and p[1][1] == 0) or
           (p[1][3] == 5 and p[2][2] == 1) or

           (p[1][3] == 6 and p[1][1] == 1)) then
      -- The S, Z and I only have two rotations
      -- Yes this is ugly.
      if direction == "RIGHT" then
         rot = rot_left
      else
         rot = rot_right
      end
   end

   -- General case rotation
   local new_p = {}
   for i, cell in pairs(p) do
      new_p[i] = {cell[1]*rot[1][1] + cell[2]*rot[1][2],
                  cell[1]*rot[2][1] + cell[2]*rot[2][2],
                  cell[3]}
   end
   return new_p
end
