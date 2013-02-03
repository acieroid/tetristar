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
   if stats.cleared[id] == nil then
      stats.cleared[id] = 0
   end
   stats.cleared[id] = stats.cleared[id] + n
end

-- Called when a block is dropped by a player
function stats.block_dropped(id)
   if stats.dropped[id] == nil then
      stats.dropped[id] = 0
   end
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

