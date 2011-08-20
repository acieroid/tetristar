plugin_subdir = "plugins/"
plugin_files = {"newplayer.lua", "say.lua", "bye.lua"}

for i, file in pairs(plugin_files) do
   dofile(plugin_subdir .. file)
end
