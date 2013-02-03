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

field = {}

function field.field(id, command, args)
   -- FIELD ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)
   for i, cell in pairs(fieldspec) do
      tetris.matrix.set_cell(user_id, cell[1], cell[2], cell[3])
   end
   tetris.matrix.commit(user_id)
   tetris.client.context_field_changed(user_id, fieldspec)
   field.update_shadow(user_id)
end

function field.piece(id, command, args)
   -- PIECE ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ", 
                                          tonumber, utils.parse_fieldspec)

   tetris.client.context_field_changed(user_id, {})

   -- Set new piece
   tetris.player.set_piece(user_id, fieldspec)
end

function field.nextpiece(id, command, args)
  -- NEXTPIECE ID FIELDSPEC
   local user_id, fieldspec = utils.split(args, " ",
                                          tonumber, utils.parse_fieldspec)
   tetris.player.set_next_piece(user_id, fieldspec)
   tetris.client.context_next_piece_changed(user_id)
end

function field.piecepos(id, command, args)
   -- PIECEPOS ID X,Y
   local user_id, rest = utils.split(args, " ",
                                     tonumber, utils.identity)
   local x, y = utils.split(rest, ",",
                            tonumber, tonumber)

   -- Set the new piece
   tetris.player.set_piece_position(user_id, {x, y})
   field.update_shadow(user_id, p)
end

function field.is_player_piece_on_cell(id, cell)
   local p = tetris.player.get_piece(id)
   p = piece.shift(p, tetris.player.get_piece_position(id))
   for j, pcell in pairs(tetris.player.get_piece(id)) do
      if (pcell[1] == cell[1] and pcell[2] == cell[2]) then
         return true
      end
   end
   return false
end

function field.is_valid(id, p)
   for i, cell in pairs(p) do
      if (tetris.matrix.get_cell(id, cell[1], cell[2]) ~= 0 and
          -- acceptable if it is on the player's piece
           not field.is_player_piece_on_cell(id, cell)) then
         return false
      end
   end
   return true
end

-- Update the displayed shadow of a player
-- TODO: only do this for the client's player
function field.update_shadow(id)
   local p = tetris.player.get_piece(id)
   if #p ~= 0 then
      p = piece.shift(p, tetris.player.get_piece_position(id))
      if field.is_valid(id, p) then
         while field.is_valid(id, p) do
            p = piece.move(p, "DOWN")
         end
         p = piece.move(p, "UP")
         tetris.client.context_set_shadow(id, p)
      else
         -- The player has lost, no shadow
         tetris.client.context_set_shadow(id, {})
      end
   end
end

tetris.plugin.register("RECV", field.field, "FIELD")
tetris.plugin.register("RECV", field.piece, "PIECE")
tetris.plugin.register("RECV", field.nextpiece, "NEXTPIECE")
tetris.plugin.register("RECV", field.piecepos, "PIECEPOS")
