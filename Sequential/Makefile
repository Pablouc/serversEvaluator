CC = gcc

INCLUDES = -I../sockets/include -I../common/include
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
DEBUG_FLAGS = -O0 -g3
LINKER_FLAGS = -L../sockets/ -lsockets -z muldefs -L../common -lcommon -lm

COMMON_FILES = ../common/linkedList.c
SERVER_FILES = sequentialServer.c

default: all

all: sequential

sockets:
	make -C ../sockets/

common:
	make -C ../common/

sequential: sequentialServer.c sockets common
	$(CC) $(COMPILER_FLAGS) -o server.o $(SERVER_FILES) $(LINKER_FLAGS)

debug: sequentialServer.c sockets common
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o server.o $(SERVER_FILES) $(LINKER_FLAGS)

clean:
	rm -rf *.o imagenrecibida*

