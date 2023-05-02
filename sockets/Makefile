CC = gcc
COMPILER_FLAGS = -Wall -Werror

default: all

all: sockets

sockets: sockets.c
	$(CC) $(COMPILER_FLAGS) -c sockets.c $(LINKER_FLAGS)
	$(AR) rcs libsockets.a sockets.o

clean:
	rm -f *.o *.a


