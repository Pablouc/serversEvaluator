CC = gcc

INCLUDES = -I../sockets/include -I../common/include -I../stadistics/
COMPILER_FLAGS = -Wall -Werror $(INCLUDES)
DEBUG_FLAGS = -O0 -g3
LINKER_FLAGS = -L../sockets/ -lsockets -z muldefs -L../common -lcommon -lm -lrt

COMMON_FILES = ../common/linkedList.c
SERVER_FILES = sequentialServer.c 
STATISTICS_FILES = ../stadistics/memory_usage.c ../stadistics/statistics.c

default: all

all: sequential

sockets:
	make -C ../sockets/

statistics: $(STATISTICS_FILES)
	$(CC) $(COMPILER_FLAGS) -c $(STATISTICS_FILES)

sequential: $(SERVER_FILES) sockets statistics
	$(CC) $(COMPILER_FLAGS) -o server.o $(SERVER_FILES) statistics.o $(COMMON_FILES) $(LINKER_FLAGS)

debug: $(SERVER_FILES) sockets statistics
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -o server.o $(SERVER_FILES) statistics.o $(COMMON_FILES) $(LINKER_FLAGS)

clean:
	rm -rf *.o imagenrecibida*
