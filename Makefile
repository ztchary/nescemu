CC=cc
CFLAGS=-Wall -Werror -Wextra -pedantic -O3 -flto
LIBS=-lSDL2
SOURCES=$(shell find src -name "*.c")
INCLUDES=$(shell find src -name "*.h")

nescemu: $(SOURCES) $(INCLUDES)
	$(CC) $(CFLAGS) -o nescemu $(SOURCES) $(LIBS)

