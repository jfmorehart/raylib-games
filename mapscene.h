#pragma once
#include "battlescene.h"
#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "UI.h"
#include "routines.h"

#include <alloca.h>
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern Island island[ISLANDCOUNT];

int shipCount = 5;
Ship ships[MAX_SHIPS];

int detectablesCount = 10;
Vector2 detectables[10];
bool isdetected[10];

void TimeRoutine(Routine *routine){

    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    float dt = 1 - fabs(0.5 - pct) / 0.5;
    timeScale = 2 * dt + 0.5;

    if(pct > 1){
        routine->isActive= false;
        timeScale = 0.1;
        printf("End Time Routine");
    }
}

Vector2 startingCameraPos;
float startingZoom;
float endZoom;
Vector2 focusTarget;
bool isZoomed;

void SwitchToBattleRoutine(Routine * routine){
    float runtime = (unscaledTime - routine->startTime);
    if(runtime >= routine->duration){
        routine->isActive= false;
        SwitchScenes(Battle);
    }
}

void FocusRoutine(Routine *routine){
    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    pct = pow(pct, 0.5);
    cameraPosition = Vector2Lerp(startingCameraPos, focusTarget, pct);
    worldScale = Lerp(startingZoom, endZoom, pct);

    if(pct > 1){
        isZoomed = !isZoomed;
        routine->isActive= false;
        printf("End Focus Routine");
    }
}

void RandomizeMap(){

    srand(time(NULL));
    for(int i = 0; i < ISLANDCOUNT; i++){
        island[i] = CreateIsland(); 
    }

    for(int i = 0; i < shipCount; i++){
        ships[i] = (Ship){true, RandomWorldPoint(), R01() * 5, 0.01};
    }   

    for(int i = 0; i < detectablesCount; i++){
        detectables[i] = RandomWorldPoint();
    } 
}


void InitMapScene(){

    // worldScale = 1;
    // cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});
    timeScale = 0.1;

    ShaderInit();
}

void MapInputLoop(){

    if(IsMouseButtonDown(0)){

        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < shipCount; i++){
                ships[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(ScreenToWorld(GetMousePosition()))){
            DrawCircleV(GetMousePosition(), 5, RED);
        }else{
            DrawCircleV(GetMousePosition(), 5, GREEN);
        }

        for(int i = 0; i < shipCount; i++){
           if(Vector2Distance(ships[i].wPos, mousePos) < 0.3){
                ships[i].selected = true;
            }
        }   
    }
    if(IsMouseButtonDown(1)){
        for(int i = 0; i < shipCount; i++){
            if(ships[i].selected){
                ships[i].tPos = mousePos;
                ships[i].hasTarget = true;
            }
        }   
    }
    if(IsKeyPressed(KEY_SPACE)){
        bool run = RunRoutine("TimeRoutine");
        printf("pressed space %u\n", run);
    }
    if(IsKeyPressed(KEY_F)){

        bool run = RunRoutine("FocusRoutine");

        if(run){
            startingCameraPos = cameraPosition;
            startingZoom = worldScale;
            focusTarget = ScreenToWorld(GetMousePosition());
            RunRoutine("SwitchToBattleRoutine");
            if(startingZoom < 0.3){
                endZoom = 1;
            }else{
                endZoom = 0.2;
            }
        }
    }

    // if(IsKeyDown(KEY_Z)){
    //     worldScale = 0.2;
    // }else{
    //     worldScale = 1;
    // }
    if(IsKeyDown(KEY_E)){
        worldScale += fixedDeltaTime * (worldScale / 0.3) * 0.1;
    }
    if(IsKeyDown(KEY_Q)){
        worldScale -= fixedDeltaTime * (worldScale / 0.3) * 0.1;
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

    if(IsKeyPressed(KEY_R)){
        for(int i = 0; i < ISLANDCOUNT; i++){
            island[i] = CreateIsland(); 
        }
        for(int i = 0; i < shipCount; i++){
            ships[i] = (Ship){true,  RandomWorldPointNoIsland(), RVec(5).x, 0.01};
        }   
        for(int i = 0; i < detectablesCount; i++){
            detectables[i] = RandomWorldPointNoIsland();
        } 
    }
}

void MapFrameLoop(){

    ClearBackground((Color){ 1, 1, 1, 255 });

    float gridSize = 0.2;
    for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}), GRAY);
    }
    for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), GRAY);
    }

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);
    mousePos_ScreenCoords.y = HEIGHT - mousePos_ScreenCoords.y;
    mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 2); 

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_ScreenCoords, 40, 0.025);
    EndOceanPass(); //flush buffer

    for(int d = 0; d < detectablesCount; d++){
        isdetected[d] = false;
    }

    PrepShipRangePass();
    for(int i = 0; i < shipCount; i++){
        DrawCircleV(WorldToScreen(ships[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < detectablesCount; d++){
            if(Vector2Distance(ships[i].wPos, detectables[d]) < SHIP_SEARCHRANGE){
                isdetected[d] = true;
            }
        }
    }
    EndOceanPass();


    for(int d = 0; d < detectablesCount; d++){
        if(isdetected[d]){
            DrawCircleV(WorldToScreen(detectables[d]), 8, RED);
        }
    }
   
    for(int i = 0; i < shipCount; i++){
        RenderShip(&ships[i]);
        SteerShip(&ships[i]);
    }

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < ISLANDCOUNT; i++){
        Render(&island[i]);
    }
    EndShaderMode();
}

