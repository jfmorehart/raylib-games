#MAC OS
MAKEFLAGS += --warn-undefined-variables
CFLAGS = -Wall -O2 $(shell pkg-config --cflags raylib) -MMD -MP -I. -Iengine -Ientities -flto                                                   
LIBS = /opt/homebrew/opt/raylib/lib/libraylib.a -framework OpenGL -framework Cocoa -framework IOKit

#WIN
WIN_CC = x86_64-w64-mingw32-gcc
WIN_CFLAGS =  -Wall -O2 -I. -Iengine -Ientities -Ivendor/raylib-win64/include -fcommon -flto -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables
WIN_LIBS = vendor/raylib-win64/lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -static -Wl,-s

all: savo

NEEDSCOMPILE = game/main.o engine/helpers.o entities/islands.o engine/routines.o entities/ships.o entities/bullets.o engine/vfx.o
WINSOURCES = $(NEEDSCOMPILE:.o=.c)    
DEPS = $(NEEDSCOMPILE:.o=.d)

savo: $(NEEDSCOMPILE)
	cc $(CFLAGS) -o savo $(NEEDSCOMPILE) $(LIBS)  -Wl,-x, -dead_strip
	strip -x savo

run: savo
	./savo

clean:
	rm -f savo $(NEEDSCOMPILE) $(DEPS)    
	rm -rf dist
	rm -f savo.exe

build-mac: savo
	mkdir -p dist/MacBuild
	cp savo dist/MacBuild
	cp -R shaders dist/MacBuild
	cp assets/Mecha.ttf dist/MacBuild
	cd dist && zip -r MacBuild.zip MacBuild

savo.exe: $(WINSOURCES)
	$(WIN_CC) $(WIN_CFLAGS) -o savo.exe $(WINSOURCES) $(WIN_LIBS) 

build-windows: savo.exe
	mkdir -p dist/WinBuild
	cp savo.exe dist/WinBuild
	cp -R shaders dist/WinBuild
	cp assets/Mecha.ttf dist/WinBuild
	cd dist && zip -r WinBuild.zip WinBuild

builds: build-windows build-mac

.PHONY: savo.exe all run clean build-mac build-windows builds

-include $(DEPS)  
