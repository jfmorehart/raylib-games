#pragma once
#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

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

Hit Intersect(Edge ab, Edge cd);

void SwitchScenes(SceneName to);
