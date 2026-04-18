#pragma once
#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>

Vector2 RVec(float scale);

float sAngle(float a1, float a2);

Vector2 VfromAngle(float angle);

float PixelsToWorld(float pixels);     
float WorldToPixels(float world);

Vector2 WorldToScreen(Vector2 worldPos);
Vector2 ScreenToWorld(Vector2 screenPos);


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




