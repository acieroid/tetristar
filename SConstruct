env = Environment()

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')
LINUX = ARGUMENTS.get('LINUX', '0')

CFLAGS += ' -I/usr/local/include -L/usr/local/lib -Ilibtetris'
CFLAGS += ' -Wall '
if DEBUG == '1':
    CFLAGS += '-g -DDEBUG '

Export('env', 'CFLAGS', 'LINUX')

if LINUX == '1':
    SConscript('libtetris/SConscript.linux')
    SConscript('server/SConscript.linux')
else:
    SConscript('libtetris/SConscript')
    SConscript('server/SConscript')

SConscript('clients/SConscript')
