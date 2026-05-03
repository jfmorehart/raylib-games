
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

#define BEAMSEGMENTS_MAX 20

typedef struct BeamHits{
    Vector2 origin;
    int hitcount;
    Hit hits[BEAMSEGMENTS_MAX];
}BeamHits;

BeamHits DrawBeam(Vector2 start, Vector2 target, float angle, int beamSegments, float beamLength, Map *m, float scaleMult);