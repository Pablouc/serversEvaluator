CC = gcc
INCLUDES = -I../common/include
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
SOURCES = sockets.c ../common/linkedList.c

default: all

all: sockets

sockets: sockets.c
	$(CC) $(COMPILER_FLAGS) -c $(SOURCES) $(LINKER_FLAGS)
	$(AR) rcs libsockets.a sockets.o

clean:
	rm -f *.o *.a


