OUT = G04T
CC = gcc
CFLAGS = $(shell sdl-config --cflags) -g
LDFLAGS = $(shell sdl-config --libs)

ENGINE_OBJECTS=sdl.o thread.o player.o mem.o view.o game.o
MAIN_OBJECTS = main.o $(ENGINE_OBJECTS)
TEST_OBJECTS = test.o $(ENGINE_OBJECTS)

all: $(OUT)
$(OUT): $(MAIN_OBJECTS)
	$(CC) -o $@ $^  ${LDFLAGS}

test: $(TEST_OBJECTS)

clean:
	rm -f *.o G04T test

