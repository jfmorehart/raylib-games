#pragma once

#include <stdio.h>
#include <dirent.h>

#include "islands.h"
#include "game/map.h"

#ifdef _WIN32
#include <direct.h> //WINDOWS
#elif __APPLE__
#include <sys/stat.h> //MACOS
#endif

FILE *GetFile(const char* path);

Map LoadMapFile(const char* path);

int GetMapCount();