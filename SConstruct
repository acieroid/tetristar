env = Environment(CPPPATH=[], LIBPATH=[], LIBS=[])

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')
LINUX = ARGUMENTS.get('LINUX', '1')
LUA = ARGUMENTS.get('LUA', 'lua')
GLIB = ARGUMENTS.get('GLIB', 'glib-2.0')
GTHREAD = ARGUMENTS.get('GTHREAD', 'gthread-2.0')
GTK = ARGUMENTS.get('GTK', 'gtk+-2.0')
CAIRO = ARGUMENTS.get('CAIRO', 'cairo')
CHECK = ARGUMENTS.get('CHECK', 'check')
RSVG = ARGUMENTS.get('RSVG', 'librsvg-2.0')
BUILD_IN_TMP = ARGUMENTS.get('BUILD_IN_TMP', '1')

env['CFLAGS'] += ['-Wall']
if DEBUG == '1':
    env['CFLAGS'] += ['-g', '-DDEBUG']

env['CPPPATH'] += ['/usr/local/include', '#/libtetris']
env['LIBPATH'] += ['#/libtetris', '/usr/local/lib']

Export('env', 'LINUX', 'LUA', 'GLIB', 'GTHREAD', 'GTK', 'CAIRO', 'CHECK', 'RSVG', 'BUILD_IN_TMP')

libtetris = SConscript('libtetris/SConscript')
Export('libtetris')
SConscript(['server/SConscript',
            'clients/SConscript',
            'tests/SConscript'])
