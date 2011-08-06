env = Environment()

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')

CFLAGS += ' -I/usr/local/include -L/usr/local/lib -Ilibtetris'
CFLAGS += ' -Wall '
if DEBUG == '1':
    CFLAGS += '-g -DDEBUG '

Export('env', 'CFLAGS')

SConscript('libtetris/SConscript')
SConscript('server/SConscript')
SConscript('clients/SConscript')
