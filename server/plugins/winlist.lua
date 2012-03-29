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
      tetris.server.send_to_all("SERVMSG WinList: [" .. i .. "] " .. 
                                   v.nick .. " with " .. v.wins .. " wins")
   end
end