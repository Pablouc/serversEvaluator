COMPILER_FLAGS = -Wall -Werror -I../sockets/include 
LINKER_FLAGS = -L../sockets/ -lsockets -z muldefs
DEBUG_FLAGS = -O0 -g3

default: all

all: client

sockets:
	make -C ../sockets/

client: client.c sockets
	$(CC) $(COMPILER_FLAGS) -o client.o client.c $(LINKER_FLAGS)

debug: client.c sockets
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o client.o client.c $(LINKER_FLAGS)

clean:
	rm -f *.o
