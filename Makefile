CC=cc
CFLAGS=-Wall -Werror

main: src/* include/*
	$(CC) $(CFLAGS) -Iinclude -o main src/* -lSDL2

