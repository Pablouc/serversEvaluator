CC = gcc

INCLUDES = -I../sockets/include -I../common/include
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
DEBUG_FLAGS = -O0 -g3
LINKER_FLAGS = -L../mesh -lmesh -L../sockets/ -lsockets -z muldefs -L../common -lcommon -lm

COMMON_FILES = ../common/linkedList.c
SERVER_FILES = preHeavyServer.c

default: all

all: sequential

sockets:
	make -C ../sockets/

mesh:
	make -C ../mesh/

common:
	make -C ../common

sequential: preHeavyServer.c sockets mesh common
	$(CC) $(COMPILER_FLAGS) -o server.o $(SERVER_FILES) $(LINKER_FLAGS)

debug: preHeavyServer.c sockets
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o server.o $(SERVER_FILES) $(LINKER_FLAGS)

clean:
	rm -rf *.o imagenrecibida*

