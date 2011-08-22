admin = {}

function admin.password(id, command, args)
   password = args
   if password == tetris.server.get_password() then
      tetris.player.set_admin(id, true)
      tetris.server.send_to_all("ADMIN " .. id)
   end
end

tetris.plugin.register("RECV", admin.password, "PASSWORD")
