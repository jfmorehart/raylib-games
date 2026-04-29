#pragma once
#include "raylib.h"
#include "helpers.h"
#include "globals.h"

#define MAXHULLPOINTS 15

typedef struct Island{
    int edgeCount; //how many are being used;
    Vector2 relativePosition;
    float scale;
    int pointCount;
    Vector2 points[MAXHULLPOINTS];
    Edge edges[MAXHULLPOINTS];
} Island;

Vector2 IslandPointToWorld(const Island *island, Vector2 objectSpace);
Vector2 WorldPointToIsland(const Island *island, Vector2 worldSpace);

int FindNextPoint(int current, const Vector2 *points);
void Render(const Island *island);
void RenderWithEdges (const Island *island, Color col);
void RenderObjectSpace (const Island *island);

Island CreateIsland();

Hit IslandIntersect(const Island *is, Edge segment);

Hit AllIslandsIntersect(const Island *islands, Edge segment);