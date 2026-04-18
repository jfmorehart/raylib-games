#pragma once
#include <raylib.h>

int WIDTH;
int HEIGHT;
Vector2 screenVec;
Vector2 mousePos;
Vector2 xBounds;

#define ISLANDCOUNT  10

#define FRAMERATE 120
int frameCount;

float deltaTime;
float appTime;