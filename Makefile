CC=cc
CFLAGS=-Wall -Werror

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lSDL2

