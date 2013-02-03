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

bonus = {}

bonus.names = {
   [8]='Add line',
   [9]='Clear line',
   [10]='Nuke',
   [11]='Random clear',
   [12]='Switch fields',
   [13]='Clear specials',
   [14]='Blockquake',
   [15]='Block bomb'
}

function bonus.receive(id, command, args)
   -- BONUSRCV ID TYPE
   local user_id, bonus = utils.split(args, " ", tonumber, tonumber)
   tetris.player.add_bonus(user_id, bonus)
   tetris.client.context_bonuses_changed(user_id)
end

function bonus.use(id, command, args)
   -- BONUS TYPE SOURCE TARGET
   local b, rest = utils.split(args, " ", tonumber, utils.identity)
   local source, target = utils.split(rest, " ", tonumber, tonumber)

   if target == 0 then
      tetris.client.chat_add_colored_text("normal",
         string.format(" * %s dropped '%s'\n",
                       tetris.player.get_nick(source),
                       bonus.names[b]))
   else
      tetris.client.chat_add_colored_text("normal",
         string.format(" * %s used '%s' on %s\n",
                       tetris.player.get_nick(source),
                       bonus.names[b],
                       tetris.player.get_nick(target)))
   end
   tetris.player.remove_bonus(source, b)
   tetris.client.context_bonuses_changed(source)
end

tetris.plugin.register("RECV", bonus.receive, "BONUSRCV")
tetris.plugin.register("RECV", bonus.use, "BONUS")