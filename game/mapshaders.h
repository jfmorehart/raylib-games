#pragma once
#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdlib.h>
#include "globals.h"

typedef struct DotShader{
    Shader shader;
    int tloc;
    int dloc;
    int camLoc;
    int wsLoc;
    int multLoc;
    int resLoc;
    int mposLoc;
    int colLoc;
}DotShader;

DotShader CreateDotShader(Shader shader);
void SetRes(DotShader *ds, Vector2 res);
void FrameRefreshShader(DotShader *ds, float time, Vector2 camPos, float worldScale, Vector2 mousePos);
void DotShaderValues(DotShader *ds, float dotSize, int multiplier, Vector3 color);

int ShaderInit();

void PrepOceanPass(Vector2 mousePos, int multiplier, float dotsize);
void EndOceanPass();
void PrepShipRangePass();
void UnloadShaders();