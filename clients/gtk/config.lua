plugin_subdir = "plugins/"
plugin_files = {"utils.lua", "chat.lua", "field.lua", "game.lua"}

for i, file in pairs(plugin_files) do
   dofile(plugin_subdir .. file)
end
