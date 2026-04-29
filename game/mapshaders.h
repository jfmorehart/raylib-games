#pragma once
#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdlib.h>
#include "globals.h"

int ShaderInit();

void PrepOceanPass(Vector2 mousePos, int multiplier, float dotsize);
void EndOceanPass();
void PrepShipRangePass();
void UnloadShaders();