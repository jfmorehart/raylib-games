
#pragma once

#include "globals.h"
#include "helpers.h"
#include "raylib.h"
#include "pools.h"

#include <math.h>
#include <stdio.h>

typedef struct Smoke{
    Vector2 wPos;
    float radius;
    PooledObject pObj;
}Smoke;


extern int smokeCount;
extern int smokeCham;
extern Smoke smokePool[];

Smoke *NextSmoke(Smoke *array, int poolSize, int *cham);

void FireSmoke(Vector2 position, float radius);

void UpdateAndRenderSmokes();
