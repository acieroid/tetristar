admin = {}

function admin.password(id, command, args)
   password = args
   if pass == tetris.server.get_password() then
      tetris.players.set_admin(id)
   end
end

tetris.plugin.register("RECV", admin.password, "PASSWORD")
