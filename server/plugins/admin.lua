admin = {}

function admin.password(id, command, args)
   password = args
   if tetris.player.is_admin(id) then
      tetris.server.send(id, "SERVMSG You are already administrator")
      return
   end
  
   if password == tetris.server.get_password() then
      tetris.player.set_admin(id, true)
      tetris.server.send_to_all(string.format("ADMIN %d", id))
      tetris.server.send(id, "SERVMSG You are now administrator");
   else
      tetris.server.send(id, "SERVMSG Wrong password");
   end
end

tetris.plugin.register("RECV", admin.password, "PASSWORD")
