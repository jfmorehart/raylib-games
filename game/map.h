
#pragma once
#include "globals.h"
#include "islands.h"
#include "raylib.h"
#include "ships.h"
// #include "shiploadouts.h"

typedef enum ObjectiveType{
    Capital,
    Radar,
    CoastalDefense
} ObjectiveType;

typedef struct Objective{
    bool alive;
    Vector2 position;
    ObjectiveType type;
    bool team;
} Objective;

#define MAX_OBJECTIVES 10

typedef struct Map{
    char filename[20];

    Island islands[ISLANDCOUNT];
    int islandLength;

    int fcount;
    Ship friendlies[MAX_SHIPS]; //todo make these some kind of ship data?

    int ecount;
    Ship enemies[MAX_SHIPS];

    int objective_count;
    Objective map_objectives[MAX_OBJECTIVES];
}Map;
