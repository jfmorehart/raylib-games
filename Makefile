CC = cc
CFLAGS = -Wall -O2
LIBS = $(shell pkg-config --libs --cflags raylib) -framework OpenGL -framework Cocoa -framework IOKit

all: raylibtest

raylibtest: main.c
	$(CC) $(CFLAGS) -o raylibtest main.c $(LIBS)

run: raylibtest
	./raylibtest

clean:
	rm -f raylibtest

.PHONY: all run clean
