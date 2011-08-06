admin = {}

function admin.password(id, command, args)
   password = args
   if pass == server.get_password() then
      tetris.players.set_admin(id)
   end
end

server.register("RECV", admin.password, "PASSWORD")
