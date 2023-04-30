COMPILER_FLAGS = -Wall -Werror

default: all

all: client

client: client.c
	$(CC) $(COMPILER_FLAGS) -o client.o client.c

clean:
	rm -f *.o
