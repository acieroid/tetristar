bonus = {}

bonus.first_bonus = #piece.cells
bonus.cells = {}
-- Bonuses are: a, c, n, r, s, b, g, q, o
for i = 1, 9 do
   bonus.cells[i] = bonus.first_bonus + i - 1
end

-- Bonus frequencies, values taken from tetrinetx
bonus.freqs = {}
bonus.freqs[1] = 32 -- a
bonus.freqs[2] = 18 -- c
bonus.freqs[3] = 1  -- n
bonus.freqs[4] = 11 -- r
bonus.freqs[5] = 3  -- s
bonus.freqs[6] = 14 -- b
bonus.freqs[7] = 1  -- g
bonus.freqs[8] = 6  -- q
bonus.freqs[9] = 14 -- o

-- Initialize the bonus distribution
bonus.distribution = {}
for i, f in ipairs(bonus.freqs) do
   for j = 0, f-1 do
      bonus.distribution[#bonus.distribution + 1] = bonus.first_bonus + i - 1
   end
end

-- Check if a cell is a bonus
function bonus.is_bonus(cell)
   return (cell > bonus.first_bonus and cell < bonus.first_bonus + #bonus.cells)
end

-- Return a random bonus
function bonus.random_bonus()
   return bonus.cells[bonus.distribution[math.random(#bonus.distribution)] - bonus.first_bonus + 1]
end

-- Return the possible positions where we can add a bonus for a player
function bonus.possible_positions(id)
   positions = {}
   for line = 0, tetris.matrix.get_height(id)-1 do
      for column = 0, tetris.matrix.get_width(id)-1 do
         cell = tetris.matrix.get_uncommited_cell(id, column, line)
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
   for i = 0, n-1 do
      b = bonus.random_bonus()
      -- add the bonus to a random position
      positions = bonus.possible_positions(id)
      if #positions ~= 0 then
         pos = positions[math.random(#positions)]
         tetris.matrix.set_cell(id, pos[1], pos[2], b)
      end
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

-- Add line bonus
function bonus.add_line(from, to)
   field.add_lines(to, 1)
   game.send_field(to)
end

-- Clear line bonus
function bonus.clear_line(from, to)
   -- Move every line down
   for line = tetris.matrix.get_height(to)-2, 0, -1 do
      for column = 0, tetris.matrix.get_width(to)-1 do
         local cell = tetris.matrix.get_uncommited_cell(to, column, line)
         tetris.matrix.set_cell(to, column, line+1, cell)
      end
   end
   game.send_field(to)
end

-- Nuke bonus
function bonus.nuke(from, to)
   for line = 0, tetris.matrix.get_height(to)-1 do
      for column = 0, tetris.matrix.get_width(to)-1 do
         tetris.matrix.set_cell(to, column, line, 0)
      end
   end
   game.send_field(to)
end

-- Clear random cells
function bonus.clear_random(from, to)
   -- That's how it's done in gtetrinet, but it might clear no blocks sometimes
   for i = 1, 10 do
      local x = math.random(tetris.matrix.get_width(to))-1
      local y = math.random(tetris.matrix.get_height(to))-1
      tetris.matrix.set_cell(to, x, y, 0)
   end
end                       

-- Switch fields bonus
function bonus.switch_fields(from, to)
   local tmp
   for line = 0,tetris.matrix.get_height(to)-1 do
      for column = 0, tetris.matrix.get_width(to)-1 do
         tmp = tetris.matrix.get_cell(to, column, line)
         tetris.matrix.set_cell(to, column, line,
                                tetris.matrix.get_cell(from, column, line))
         tetris.matrix.set_cell(from, column, line, tmp)
      end
   end
   game.send_field(from)
   game.send_field(to)
end

-- Clear specials bonus
function bonus.clear_specials(from, to)
   for line = 0, tetris.matrix.get_height(to)-1 do
      for column = 0, tetris.matrix.get_width(to)-1 do
         if bonus.is_bonus(tetris.matrix.get_uncommited_cell(to, column, line)) then
            tetris.matrix.set_cell(to, column, line, piece.random_cell())
         end
      end
   end
   game.send_field(to)
end

-- Contains the actions to do when a bonus is used
bonus.actions = {
   bonus.add_line,       -- a
   bonus.clear_line,     -- c
   bonus.nuke,           -- n
   bonus.clear_random,   -- r
   bonus.switch_fields,  -- s
   bonus.clear_specials, -- b
   bonus.dummy_bonus,    -- g
   bonus.dummy_bonus,    -- q
   bonus.dummy_bonus     -- o
}

-- Called when a player uses a bonus   
function bonus.use_bonus(id, command, args)
   local target = tonumber(args)
   local b = tetris.player.first_bonus(id)
   if b ~= 0 and tetris.player.exists(target) or target == 0 then
      tetris.player.drop_bonus(id)
      if target ~= 0 then
         bonus.actions[b - bonus.first_bonus + 1](id, target)
      end
      tetris.server.send_to_all(
         string.format("BONUS %d %d %d", b, id, target))
   end
end

tetris.plugin.register("RECV", bonus.use_bonus, "BONUS")