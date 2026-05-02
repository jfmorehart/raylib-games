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

#define STRINGARRAY_STRLEN 20
#define STRINGGARY_MAXCHARS 800
#define STRINGARRAY_MAXSTRINGS 40

typedef struct StringArray{
    int numStrings;
    char chars[STRINGGARY_MAXCHARS];
    int stringIndices[40];
}StringArray;

void AssignName(Map *map, const char * name);

int AppendStringToStrArr(const char* str, StringArray * strArr);

FILE *GetFile(const char* path);

Map LoadMapFile(const char* path);

int GetMapCount();

char *StringAt(StringArray *strArr, int index);

void PrintAllStringsInStrArr(StringArray * strArr);

StringArray GetMapNames();