env = Environment(CPPPATH=[], LIBPATH=[], LIBS=[])

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')
LINUX = ARGUMENTS.get('LINUX', '0')

env['CFLAGS'] += ['-Wall']
if DEBUG == '1':
    env['CFLAGS'] += ['-g', '-DDEBUG']

env['CPPPATH'] += ['/usr/local/include', '#/libtetris']
env['LIBPATH'] += ['#/libtetris', '/usr/local/lib']

if LINUX == '1':
    LUA = 'lua'
else:
    LUA = 'lua-5.1'

GLIB = 'glib-2.0'
GTK = 'gtk+-2.0'

Export('env', 'LINUX', 'LUA', 'GLIB', 'GTK')

SConscript(['libtetris/SConscript', 
            'server/SConscript',
            'clients/SConscript'])
