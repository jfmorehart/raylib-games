#MAC OS
MAKEFLAGS += --warn-undefined-variables
CFLAGS = -Wall -O2 $(shell pkg-config --cflags raylib) -MMD -MP -I. -Iengine -Ientities                                                    
LIBS = /opt/homebrew/opt/raylib/lib/libraylib.a -framework OpenGL -framework Cocoa -framework IOKit

#WIN
WIN_CC = x86_64-w64-mingw32-gcc
WIN_CFLAGS =  -Wall -O2 -I. -Iengine -Ientities -Ivendor/raylib-win64/include -fcommon
WIN_LIBS = vendor/raylib-win64/lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -static

all: savo

NEEDSCOMPILE = engine/main.o engine/helpers.o entities/islands.o engine/routines.o entities/ships.o entities/bullets.o engine/vfx.o
WINSOURCES = $(NEEDSCOMPILE:.o=.c)    
DEPS = $(NEEDSCOMPILE:.o=.d)

savo: $(NEEDSCOMPILE)
	cc $(CFLAGS) -o savo $(NEEDSCOMPILE) $(LIBS)  -Wl,-x, -dead_strip

run: savo
	./savo

clean:
	rm -f savo $(NEEDSCOMPILE) $(DEPS)    

build-mac: 
	mkdir -p MacBuild — -p
	cp savo MacBuild
	cp -R shaders MacBuild
	cp Mecha.ttf MacBuild

savo.exe: $(WINSOURCES)
	$(WIN_CC) $(WIN_CFLAGS) -o savo.exe $(WINSOURCES) $(WIN_LIBS) 

build-windows: savo.exe
	mkdir -p WinBuild
	cp savo.exe WinBuild
	cp -R shaders WinBuild
	cp Mecha.ttf WinBuild

.PHONY: savo.exe all run clean build-mac build-windows

-include $(DEPS)  
