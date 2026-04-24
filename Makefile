MAKEFLAGS += --warn-undefined-variables
CFLAGS = -Wall -O2 $(shell pkg-config --cflags raylib)                                                                                
LIBS = /opt/homebrew/opt/raylib/lib/libraylib.a -framework OpenGL -framework Cocoa -framework IOKit

all: raylibtest

HFILES = mapshaders.h helpers.h islands.h globals.h ships.h UI.h mapscene.h battlescene.h routines.h

main.o : main.c $(HFILES)
helpers.o : helpers.c $(HFILES)
islands.o : islands.c $(HFILES)
routines.o : routines.c $(HFILES)

NEEDSCOMPILE = main.o helpers.o islands.o routines.o

raylibtest: $(NEEDSCOMPILE)
	cc $(CFLAGS) -o raylibtest $(NEEDSCOMPILE) $(LIBS)

run: raylibtest
	./raylibtest 

clean:
	rm -f raylibtest $(NEEDSCOMPILE)

.PHONY: all run clean
