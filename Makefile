CMDS=ppt
CC=gcc
CFLAGS=-Wall -O2

all: ${CMDS}

sparse:
	sparse *.c

clean:
	rm -f ${CMDS}
