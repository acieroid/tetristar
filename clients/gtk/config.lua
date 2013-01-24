plugin_subdir = "plugins/"
plugin_files = {"utils.lua", "chat.lua", "piece.lua", "field.lua", "game.lua",
                "bonus.lua"}

for i, file in pairs(plugin_files) do
   dofile(plugin_subdir .. file)
end
