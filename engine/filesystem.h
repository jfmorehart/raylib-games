#pragma once

#include <stdio.h>
#include <dirent.h>

#include "islands.h"
#include "game/map.h"
#include "fleet.h"

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

typedef struct PolyPoly{
    char filename[20];
    Vector2 polyCenter;
    float polyScale;
    Island islands[ISLANDCOUNT];
    int islandLength;
}PolyPoly;

typedef enum FileType{
    ShipFile,
    TaskForceFile,
    IslandFile,
    MapFile,
    LevelFile,
    PolyPolyFile,
    FleetFile

} FileType;



void AssignName(char filename[STRINGARRAY_STRLEN], const char * name);

int AppendStringToStrArr(const char* str, StringArray * strArr);

bool FileCheck(const char* path);

MapRecord LoadMapRecord(const char* path); 
Map LoadMapFile(const char* path);

PolyPoly LoadPolyFile(const char* path);

Fleet LoadFleetFile(const char* path);

int GetMapCount();

char *StringAt(StringArray *strArr, int index);

void PrintAllStringsInStrArr(StringArray * strArr);

StringArray GetMapNames();
StringArray GetPolyNames();

void ReAppendSuffix(char* dest, char * toread, const char * toappend);

void RehydrateMap(Map * toreh, MapRecord * record);
MapRecord DehydrateMap(Map * dehydrate);