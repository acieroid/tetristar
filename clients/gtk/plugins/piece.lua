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
