MAKEFLAGS += --warn-undefined-variables
CFLAGS = -Wall -O2 $(shell pkg-config --cflags raylib)                                                                                
LIBS = $(shell pkg-config --libs raylib) -framework OpenGL -framework Cocoa -framework IOKit

all: raylibtest

HFILES = mapshaders.h helpers.h islands.h globals.h

main.o : main.c $(HFILES)
helpers.o : helpers.c $(HFILES)
NEEDSCOMPILE = main.o helpers.o

raylibtest: $(NEEDSCOMPILE)
	cc $(CFLAGS) -o raylibtest $(NEEDSCOMPILE) $(LIBS)

run: raylibtest
	./raylibtest 

clean:
	rm -f raylibtest $(NEEDSCOMPILE)

.PHONY: all run clean
