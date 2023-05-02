INCLUDES = -I../common/include -I../sockets/include
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
LINKER_FLAGS = -L../sockets/ -lsockets -z muldefs -L../common -lcommon
DEBUG_FLAGS = -O0 -g3
SOURCES = client.c

default: all

all: client

sockets:
	make -C ../sockets/

common:
	make -C ../common/

client: client.c sockets common
	$(CC) $(COMPILER_FLAGS) -o client.o $(SOURCES) $(LINKER_FLAGS)

debug: client.c sockets common
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o client.o client.c $(LINKER_FLAGS)

clean:
	rm -f *.o
