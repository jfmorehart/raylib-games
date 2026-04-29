#pragma once
#include "raylib.h"
#include "raymath.h"
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

#include <math.h>       
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

extern Map currentMap;

extern Vector2 worldZero;

int allShipsIncludedCount;
Ship *allShipsIncludedInScene[MAX_SHIPS + MAX_SHIPS];

int bulletCham;
int bulletCount = 100;
Bullet bulletPool[100];

int smokeCham;
int smokeCount = 100;
Smoke smokePool[100];

int splashCham;
int splashCount = 100;
Smoke splashPool[100];

extern Vector2 startingCameraPos;
extern float startingZoom;
extern float endZoom;
extern Vector2 focusTarget;

extern Shader islandShader_frag;
extern Shader oceanShader_frag;
extern Shader ship_frag;

void InitBattleScene(){

    timeScale = 1;

    //setup Ship CONSTANTS (overwrite from mapscene)
    int resLoc = GetShaderLocation(ship_frag, "multiplier");   
    int multiplier = 120;
    SetShaderValue(ship_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    resLoc = GetShaderLocation(ship_frag, "dotsize");   
    float dotsize = 0.2;
    SetShaderValue(ship_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    //determine which ships are included in the scene
    allShipsIncludedCount = 0;
    for(int i = 0; i < currentMap.fcount; i++){
        currentMap.friendlies[i].includedInScene = false;
        if(!currentMap.friendlies[i].alive)continue;
        if(IsOnScreen(currentMap.friendlies[i].wPos)){
            currentMap.friendlies[i].includedInScene = true;
            allShipsIncludedInScene[allShipsIncludedCount] = &currentMap.friendlies[i];
            allShipsIncludedCount++;
        }
    }
    for(int i = 0; i < currentMap.ecount; i++){
        currentMap.enemies[i].includedInScene = false;
        if(!currentMap.enemies[i].alive)continue;
        if(IsOnScreen(currentMap.enemies[i].wPos)){
            currentMap.enemies[i].includedInScene = true;
            allShipsIncludedInScene[allShipsIncludedCount] = &currentMap.enemies[i];
            allShipsIncludedCount++;
        }
    }
}

void BattleFrameLoop(){
    ClearBackground((Color){ 5, 5, 20, 255 });

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_fragCoords, 100, 0.02);
    
    EndOceanPass(); //flush buffer

    // float gridSize = 0.2;
    // for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}), GRAY);
    // }
    // for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), GRAY);
    // }


    for(int d = 0; d < currentMap.ecount; d++){
        currentMap.enemies[d].isVisible = false;
    }

    PrepShipRangePass();
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < currentMap.ecount; d++){
            if(!currentMap.enemies[d].alive || !currentMap.enemies[d].includedInScene)continue;
            if(Vector2Distance(currentMap.friendlies[i].wPos, currentMap.enemies[d].wPos) < SHIP_SEARCHRANGE){
                currentMap.enemies[d].isVisible = true;
            }
        }
    }
    EndShaderMode();

    int mLoc = GetShaderLocation(ship_frag, "multiplier");   
    int mult = 150;
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);

    int dLoc = GetShaderLocation(ship_frag, "dotsize");   
    float dotsize = 0.2;
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    //Set color red
    int colorLocation = GetShaderLocation(ship_frag, "dotcolor");   

    Vector3 col = (Vector3){1, 0, 0};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int d = 0; d < currentMap.ecount; d++){
        if(currentMap.enemies[d].isVisible && currentMap.enemies[d].alive && currentMap.enemies[d].includedInScene){
            RenderShip(&currentMap.enemies[d], 0.3);
        }
    }
    EndShaderMode();

    //Set color white
    col = (Vector3){1, 1, 1};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        RenderShip(&currentMap.friendlies[i], 0.3);
        SteerShip(&currentMap.friendlies[i], 0.05, false, currentMap.islands);
    }
    EndShaderMode();

    //trace ship stuff?
    mLoc = GetShaderLocation(ship_frag, "multiplier");   
    mult = 90;
    dotsize = 0.03;
    dLoc = GetShaderLocation(ship_frag, "dotsize");   
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);
    col = (Vector3){0.8, 0.8, 0.8};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        ShipCombat(&currentMap.friendlies[i], currentMap.enemies, currentMap.ecount);
    }
    for(int i = 0; i < currentMap.ecount; i++){
        if(!currentMap.enemies[i].alive || !currentMap.enemies[i].includedInScene)continue;
        ShipCombat(&currentMap.enemies[i], currentMap.friendlies, currentMap.fcount);
    }
    EndShaderMode();

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < currentMap.islandLength; i++){
        Render(&currentMap.islands[i]);
    }
    EndShaderMode();

    //BULLETS
    mLoc = GetShaderLocation(ship_frag, "multiplier");   
    mult = 230;
    dotsize = 0.15;
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);
    col = (Vector3){0.7, 0.7, 0.7};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    UpdateAndRenderBullets(bulletPool, bulletCount, allShipsIncludedInScene, allShipsIncludedCount);
    EndShaderMode();

    //Explosions!
    col = (Vector3){1, 0.8, 0};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    UpdateAndRenderBlobs(smokePool, smokeCount);
    EndShaderMode();

    //Splashes
    dotsize = 0.3;
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);
    mult = 180;
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);
    col = (Vector3){0.1, 0.1, 0.3};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    UpdateAndRenderBlobs(splashPool, splashCount);
    EndShaderMode();

    if(IsMouseButtonDown(0)){

        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < currentMap.fcount; i++){
                currentMap.friendlies[i].selected = false;
                currentMap.enemies[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(mousePos)){
            DrawCircleV(mousePos_ScreenCoords, 5, RED);
        }else{
            DrawCircleV(mousePos_ScreenCoords, 5, GREEN);
        }

        for(int i = 0; i < currentMap.fcount; i++){
            if(!currentMap.friendlies[i].alive)continue;
            if(Vector2Distance(currentMap.friendlies[i].wPos, mousePos) < 0.1){
                currentMap.friendlies[i].selected = true;
            }
        }   
    }
    if(IsKeyDown(KEY_D)){
        cameraPosition.x += fixedDeltaTime * worldScale;
    }
    if(IsKeyDown(KEY_A)){
        cameraPosition.x -= fixedDeltaTime * worldScale;
    }
     if(IsKeyDown(KEY_W)){
        cameraPosition.y += fixedDeltaTime * worldScale;
    }
    if(IsKeyDown(KEY_S)){
        cameraPosition.y -= fixedDeltaTime * worldScale;
    }
    
    if(IsKeyDown(KEY_F)){
        InitMapScene();
        bool run = RunRoutine("FocusRoutine");
        if(run){
            startingCameraPos = cameraPosition;
            startingZoom = worldScale;
            focusTarget = worldZero;
            if(startingZoom < 0.5){
                endZoom = 2;
            }else{
                endZoom = 0.4;
            }
        }
        currentScene = MapScene;
    }


    if(IsMouseButtonDown(1)){
        for(int i = 0; i < currentMap.fcount; i++){
            if(currentMap.friendlies[i].selected){
                currentMap.friendlies[i].moveTargetPosition = mousePos;
                currentMap.friendlies[i].hasMoveTarget = true;
            }
        }   
    }
}