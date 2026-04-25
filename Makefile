MAKEFLAGS += --warn-undefined-variables
CFLAGS = -Wall -O2 $(shell pkg-config --cflags raylib) -MMD -MP -I. -Iengine -Ientities 
                                                                             
LIBS = /opt/homebrew/opt/raylib/lib/libraylib.a -framework OpenGL -framework Cocoa -framework IOKit

all: raylibtest

#HFILES = mapshaders.h helpers.h islands.h globals.h ships.h UI.h mapscene.h battlescene.h routines.h

#main.o : main.c #$(HFILES)
#helpers.o : helpers.c #$(HFILES)
#islands.o : islands.c #$(HFILES)
#routines.o : routines.c #$(HFILES)


NEEDSCOMPILE = engine/main.o engine/helpers.o entities/islands.o engine/routines.o entities/ships.o entities/bullets.o engine/vfx.o
DEPS = $(NEEDSCOMPILE:.o=.d)

raylibtest: $(NEEDSCOMPILE)
	cc $(CFLAGS) -o raylibtest $(NEEDSCOMPILE) $(LIBS)  -Wl,-x, -dead_strip

run: raylibtest
	./raylibtest 

clean:
	rm -f raylibtest $(NEEDSCOMPILE) $(DEPS)    

.PHONY: all run clean

-include $(DEPS)  
