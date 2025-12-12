CC=cc
CFLAGS=-Wall -Werror

main:
	$(CC) $(CFLAGS) -Iinclude -o main src/*.c src/mappers/*.c -lSDL2

