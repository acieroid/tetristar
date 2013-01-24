bonus = {}

function bonus.receive(id, command, args)
   -- BONUSRCV ID TYPE
   local user_id, bonus = utils.split(args, " ", tonumber, tonumber)
   tetris.player.add_bonus(user_id, bonus)
   -- tetris.client.context_bonus_changed(id)
   print("Received bonus: " .. bonus)
end

function bonus.use(id, command, args)
   -- BONUS TYPE SOURCE TARGET
   local bonus, rest = utils.split(args, " ", tonumber, utils.identity)
   local source, target = utils.split(rest, " ", tonumber, tonumber)

   tetris.client.chat_add_text(
      string.format(" * %s used '%s' on %s",
                    tetris.player.get_nick(source),
                    bonus.get_bonus_name(bonus),
                    tetris.player.get_nick(target)))
end

tetris.plugin.register("RECV", bonus.receive, "BONUSRCV")
tetris.plugin.register("RECV", bonus.use, "BONUS")