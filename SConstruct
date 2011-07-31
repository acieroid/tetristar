env = Environment()

CFLAGS = ARGUMENTS.get('CFLAGS', '')
DEBUG = ARGUMENTS.get('DEBUG', '1')

CFLAGS += ' '
if DEBUG == '1':
    CFLAGS += '-g -DDEBUG '

Export('env', 'CFLAGS')

SConscript('server/SConscript')
