#pragma once
#include "raylib.h"
#include "globals.h"
// #include "game/map.h"

float R01();
Vector2 RVec(float scale);
Vector2 RVec_Perlin(float index, float wobbleFreq);
Vector2 RandomWorldPoint();

float SignedAngle(float a1, float a2);

Vector2 VfromAngle(float angle);

float PixelsToWorld(float pixels);     
float WorldToPixels(float world);

Vector2 WorldToScreen(Vector2 worldPos);
Vector2 ScreenToWorld(Vector2 screenPos);

bool IsOnScreen(Vector2 worldPoint);

typedef struct Island Island;// name;
Island *WhatIslandIsThis(Vector2 wPoint, Island *toCheckArray, int islandLength);

Vector2 NearestPointOnSegment(Vector2 la, Vector2 lb, Vector2 point);
bool PointIslandCheck(Vector2 point, const Island *is);
bool IsPointWithinIslands(Vector2 wPoint);

Vector2 RandomWorldPointNoIsland();

typedef struct{
    // from A to B
    Vector2 a;
    Vector2 b;
}Edge;

typedef struct{
    bool hit;
    Vector2 hitPosition;
} Hit;

typedef struct Ship Ship;
float DistanceToShipEdge(const Ship *ship, Vector2 point, float scaleMult);

Hit Intersect(Edge ab, Edge cd);

void SwitchScenes(SceneName to);

typedef struct Map Map;

Hit RayAllShipsIntersect(Edge ray, Ship * allships, int shipCount, float scaleMult);
Hit IntersectIslandsAndShips(Vector2 start, Vector2 angle, Map *m, float scaleMult);