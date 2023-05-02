INCLUDES = -I../common/include -I../sockets/include
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
LINKER_FLAGS = -L../sockets/ -lsockets -z muldefs
DEBUG_FLAGS = -O0 -g3
SOURCES = client.c ../common/linkedList.c

default: all

all: client

sockets:
	make -C ../sockets/

client: client.c sockets
	$(CC) $(COMPILER_FLAGS) -o client.o $(SOURCES) $(LINKER_FLAGS)

debug: client.c sockets
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o client.o client.c $(LINKER_FLAGS)

clean:
	rm -f *.o
