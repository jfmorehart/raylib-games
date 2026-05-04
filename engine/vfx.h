
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
void FireSplash(Vector2 position, float radius);

//all smoke-likes (smokes, splashes)
void UpdateAndRenderBlobs(Smoke *pool, int count);

#define BEAMSEGMENTS_MAX 50


void DrawBeam(Vector2 start, Vector2 target, float angle, int beamSegments, float beamLength, Map *m, float scaleMult, float illumbrightness);