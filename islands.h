#pragma once
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"


#include <math.h>       
#include <stdio.h>
#include <stdlib.h>

#define MAXHULLPOINTS 15

typedef struct{
    int edgeCount; //how many are being used;
    Vector2 relativePosition;
    float scale;
    Vector2 points[MAXHULLPOINTS];
    Edge edges[MAXHULLPOINTS];
} Island;

Island island[ISLANDCOUNT];

Vector2 IslandPointToWorld(const Island *island, Vector2 objectSpace);

int FindNextPoint(int current, const Vector2 *points);
void Render (const Island *island);

Island CreateIsland();

Hit IslandIntersect(const Island *is, Edge segment);

Hit AllIslandsIntersect(const Island *islands, Edge segment);