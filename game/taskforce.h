
#pragma once
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "ships.h"
#include "fleet.h"

typedef struct TaskForce {
    bool team;
    char name[20];
    Vector2 position;
    float min_speed;
    int shipCount;
    Ship * ships[MAX_SHIPS_IN_TF];
    Vector2 destination;
    bool selected;
} TaskForce;

void CompleteRehydrateMap(Map * toreh);
void RehydrateTaskForces(int * tf_length, TaskForce *torehydrate, Fleet * friendly, Fleet * enemy, Map * placeShipsIn);
void DehydrateTaskForces(int tf_count, TaskForce * tfs, Fleet * friendly, Fleet * enemy);
void TFShipsWorldToLocal();
void TFShipsLocalToWorld();
void CreateTaskForcesFromMapFile(Map *map);


