env = Environment(CPPPATH=[], LIBPATH=[], LIBS=[])

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')
LINUX = ARGUMENTS.get('LINUX', '1')
LUA = ARGUMENTS.get('LUA', 'lua')
BUILD_IN_TMP = ARGUMENTS.get('BUILD_IN_TMP', '1')

env['CFLAGS'] += ['-Wall']
if DEBUG == '1':
    env['CFLAGS'] += ['-g', '-DDEBUG']

env['CPPPATH'] += ['/usr/local/include', '#/libtetris']
env['LIBPATH'] += ['#/libtetris', '/usr/local/lib']

GLIB = 'glib-2.0'
GTHREAD = 'gthread-2.0'
GTK = 'gtk+-2.0'
CAIRO = 'cairo'
CHECK = 'check'
RSVG = 'librsvg-2.0'

Export('env', 'LINUX', 'LUA', 'GLIB', 'GTHREAD', 'GTK', 'CAIRO', 'CHECK', 'RSVG', 'BUILD_IN_TMP')

libtetris = SConscript('libtetris/SConscript')
Export('libtetris')
SConscript(['server/SConscript',
            'clients/SConscript',
            'tests/SConscript'])
