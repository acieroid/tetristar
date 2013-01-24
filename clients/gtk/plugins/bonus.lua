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
      tetris.client.chat_add_text(
         string.format(" * %s dropped '%s'",
                       tetris.player.get_nick(source),
                       bonus.names[b])
   else
      tetris.client.chat_add_text(
         string.format(" * %s used '%s' on %s",
                       tetris.player.get_nick(source),
                       bonus.names[b],
                       tetris.player.get_nick(target)))
   end
   tetris.player.remove_bonus(source, b)
end

tetris.plugin.register("RECV", bonus.receive, "BONUSRCV")
tetris.plugin.register("RECV", bonus.use, "BONUS")