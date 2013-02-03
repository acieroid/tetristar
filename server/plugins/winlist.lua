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

winlist = {}
winlist.winners = {}

function winlist.add(id)
   last = winlist.winners[tetris.player.get_nick(id)]
   if last then
      winlist.winners[tetris.player.get_nick(id)] = last+1
   else
      winlist.winners[tetris.player.get_nick(id)] = 1
   end
   winlist.send(5)
end

function winlist.compare(a, b)
   return a.wins < b.wins
end

function winlist.send(n)
   -- Send the n most winning nicks to everyone
   local sorted = {}
   for n, w in pairs(winlist.winners) do
      table.insert(sorted, {nick = n, wins = w})
   end

   table.sort(sorted, winlist.compare)
   for i, v in ipairs(sorted) do
      tetris.server.send_to_all(
         string.format("SERVMSG WinList: [%d] %s with %d wins",
                       i, v.nick, v.wins))
   end
end