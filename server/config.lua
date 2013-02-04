config = {}

config.server = 'localhost'
config.port = 12345
config.plugins = {'plugins/utils.lua', 'plugins/stats.lua',
                  'plugins/connection.lua', 'plugins/im.lua', 
                  'plugins/piece.lua', 'plugins/bonus.lua',
                  'plugins/field.lua', 'plugins/admin.lua',
                  'plugins/winlist.lua', 'plugins/game.lua',
                  'plugins/kept.lua'}
config.password = 'foo'
config.datadir = '/tmp/'
