#pragma once
// #include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "UI.h"
#include "pools.h"
#include "bullets.h"
#include "vfx.h"
#include "map.h"
#include "mapscene.h"
#include "cutscene.h"

#include "rlgl.h"


#include <math.h>       
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

extern Map mapFromDisk;

extern Vector2 worldZero;
extern PolyPoly cruiser;

int allShipsIncludedCount;
Ship *allShipsIncludedInScene[MAX_SHIPS + MAX_SHIPS];

extern Vector2 startingCameraPos;
extern float startingZoom;
extern float endZoom;
extern Vector2 focusTarget;

extern DotShader islandShader;
extern DotShader oceanShader;
extern DotShader generalShader;
extern DotShader explosionShader;
extern DotShader lightShader;
extern DotShader illuminatedShader;

bool battleOver;
bool wonBattle;
float battleOverTime;

typedef struct FireStack{
    bool alive;
    Ship * attached;
    float amount;
    Vector2 localOffset;
}FireStack;

void ApplyFireStacks(Ship * toship, int amount);

void InitBattleScene();

void BattleFrameLoop();

void BattleUIRender();