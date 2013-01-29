stats = {}

-- Number of piece dropped by each player
stats.dropped = {}
-- Number of lines cleared by each player
stats.cleared = {}
-- Start times of the players
stats.start_time = {}
-- End times of the players
stats.end_time = {}

-- Called when a player started to play
function stats.player_started(id)
   stats.start_time[id] = os.time()
   stats.dropped[id] = 0
   stats.cleared[id] = 0
end

-- Called when a player lost
function stats.player_finished(id)
   stats.end_time[id] = os.time()
end

-- Called when n line are cleared by a player
function stats.lines_cleared(id, n)
   stats.cleared[id] = stats.cleared[id] + n
end

-- Called when a block is dropped by a player
function stats.block_dropped(id)
   stats.dropped[id] = stats.dropped[id] + 1
end

-- Send the statistics to all the clients
function stats.send()
   for i, id in pairs(tetris.player.all()) do
      local cleared = stats.cleared[id]
      local dropped = stats.dropped[id]
      local start_time = stats.start_time[id]
      local end_time = stats.end_time[id]
      if start_time ~= nil and end_time ~= nil then
         local drop_per_min = (60*dropped)/(end_time-start_time)
         tetris.server.send_to_all(
            string.format("SERVMSG Statistics for %s: %d lines cleared, %d blocks dropped, %.2f block drop/min",
                          tetris.player.get_nick(id), cleared, dropped, drop_per_min))
      end
   end
   -- Reinitializes stats
   stats.dropped = {}
   stats.cleared = {}
   stats.start_time = {}
   stats.end_time = {}
end

