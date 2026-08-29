#pragma once
#include "globals.h"
#include "helpers.h"
#include <stdio.h>
#include "raymath.h"
#include "rlgl.h"

#include "mapshaders.h"
#include "islands.h"
#include "filesystem.h"

#define HORIZON_Y 0


typedef enum Cutscene{
    ShipsOnHorizon,
    LoneCaptain,
    SinkingFriendly,
    SinkingEnemy
} Cutscene;

void InitCutScene();

Cutscene activeCutscene;

void SetCutscene(Cutscene cut);

void DrawHorizonObj(float azimuth, float worldWidth, float worldHeight, float distance);

void RenderPoly(PolyPoly todraw);
void RenderPolyAsUI(PolyPoly todraw);

void DrawShipsOnHorizon();
void DrawCaptain();


void UpdateCutScene();

void CutSceneUIUpdate();