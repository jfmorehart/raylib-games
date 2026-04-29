
#pragma once
#include "globals.h"
#include "islands.h"
#include "raylib.h"
#include "ships.h"
// #include "shiploadouts.h"

typedef struct Map{
    Island islands[ISLANDCOUNT];
    int islandLength;

    int fcount;
    Ship friendlies[MAX_SHIPS]; //todo make these some kind of ship data?

    int ecount;
    Ship enemies[MAX_SHIPS];
}Map;
