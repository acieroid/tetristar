bonus = {}

bonus.first_bonus = #piece.pieces
bonus.cells = {}
-- Bonuses are: a, c, n, r, s, b, g, q, o
for i = 0, 9 do
   bonus.cells[i] = bonus.first_bonus + i
end

-- Maximum number of bonuses a player can receive when he clears a line
bonus.max_bonuses = 2

-- Check if a cell is a bonus
function bonus.is_bonus(cell)
   return (cell > bonus.first_bonus and cell < bonus.first_bonus + #bonus.cells)
end

-- Return a random bonus
function bonus.random_bonus()
   return bonus.cells[math.random(#bonus.cells)]
end

-- Return the possible positions where we can add a bonus for a player
function bonus.possible_positions(id)
   positions = {}
   for line = 0, tetris.matrix.get_height(id)-1 do
      for column = 0, tetris.matrix.get_width(id)-1 do
         cell = tetris.matrix.get_cell(id, column, line)
         -- The only possible positions are where a cell is set and is
         -- not a bonus
         if cell ~= 0 and cell < bonus.first_bonus then
            table.insert(positions, {column, line})
         end
      end
   end
   return positions
end

-- Add random bonuses to a player's matrix
function bonus.add_bonuses(id, n)
   -- This line can be uncommented to have a different behaviour from tetrinet
   -- n = math.random(bonus.max_bonuses) -- number of bonuses to add
   for i = 0, n do
      b = bonus.random_bonus()
      -- add the bonus to a random position
      positions = bonus.possible_positions(id)
      pos = positions[math.random(#positions)]
      tetris.matrix.set_cell(id, pos[1], pos[2], b)
   end
end

-- Called when some lines are cleared by a player
function bonus.lines_cleared(id, n)
   bonus.add_bonuses(id, n)
end

-- Called when a player receives a bonus
function bonus.receive_bonus(id, bonus)
   tetris.player.add_bonus(id, bonus)
   -- The bonus information is sent only to the bonus owner
   tetris.server.send(id,
                      string.format("BONUSRCV %d %d", id, bonus))
end

-- A bonus that does nothing
function bonus.dummy_bonus(from, to)
end

-- Contains the actions to do when a bonus is used
bonus.actions = {
   bonus.dummy_bonus, -- a
   bonus.dummy_bonus, -- c
   bonus.dummy_bonus, -- n
   bonus.dummy_bonus, -- r
   bonus.dummy_bonus, -- s
   bonus.dummy_bonus, -- b
   bonus.dummy_bonus, -- q
   bonus.dummy_bonus  -- o
}

-- Called when a player uses a bonus   
function bonus.use_bonus(id, command, args)
   local bonus, target = utils.split(args, " ", tonumber, tonumber)
   if bonus.is_bonus(bonus) and tetris.player.has_bonus(id, bonus) then
      tetris.players.remove_bonus(id, bonus)
      bonus.actions[bonus - bonus.first_bonus](id, target)
      tetris.server.send_to_all(
         string.format("BONUS %d %d %d", bonus, id, target))
   end
end

tetris.plugin.register("RECV", bonus.use_bonus, "BONUS")