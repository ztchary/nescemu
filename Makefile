CC=cc
CFLAGS=-Wall -Werror

.PHONY: cputest

main:
	$(CC) $(CFLAGS) -Iinclude -o main src/*.c src/mappers/*.c -lSDL2

cputest:
	$(CC) $(CFLAGS) -Icputest/include -o cputest/cputest cputest/src/*.c src/cpu.c
	cputest/cputest cputest/nestest.nes > cputest/nestest.log2
	cmp -s cputest/nestest.log cputest/nestest.log2 && echo -e "\e[32mcputest passed\e[m" || echo -e "\e[31mcputest failed\e[m"

