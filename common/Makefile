COMPILER_FLAGS = -Wall -Werror
LINKER_FLAGS = -lm
DEBUG_FLAGS = -O0 -g3

default: all

all: sobel linkedList build_library

sobel: sobel.c
	$(CC) $(COMPILER_FLAGS) -c sobel.c $(LINKER_FLAGS)

linkedList: linkedList.c
	$(CC) $(COMPILER_FLAGS) $(DEBUG_FLAGS) -c linkedList.c $(LINKER_FLAGS)

build_library: sobel linkedList
	ar rcs libcommon.a sobel.o linkedList.o

clean:
	rm -rf *.o *.a
