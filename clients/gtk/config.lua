plugin_subdir = "plugins/"
plugin_files = {"utils.lua", "newplayer.lua", "say.lua", "bye.lua", 
   "field.lua"}

for i, file in pairs(plugin_files) do
   dofile(plugin_subdir .. file)
end
