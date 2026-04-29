// #include "battlescene.h"
#include "bullets.h"
#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "routines.h"
#include "shiploadouts.h"
#include "map.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

Map currentMap;

extern Battery DestroyerLoadout[SHIP_MAXBATTERIES];

extern Shader oceanShader_frag;
extern Shader ship_frag;
extern Shader islandShader_frag;

Ship defaultShip;

void TimeRoutine(Routine *routine){

    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    float dt = 1 - fabs(0.5 - pct) / 0.5;
    timeScale = 2 * dt + 0.5;

    if(pct > 1){
        routine->isActive= false;
        timeScale = 0.1;
        // printf("End Time Routine");
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
        // printf("End Focus Routine");
    }
}
void RandomizeMap(){

    MakeLoadouts();

    srand(time(NULL));
    for(int i = 0; i < ISLANDCOUNT; i++){
        currentMap.islands[i] = CreateIsland(); 
    }
    currentMap.islandLength  = ISLANDCOUNT;
    
    currentMap.fcount = MAX_SHIPS;
    for(int i = 0; i < currentMap.fcount; i++){

        currentMap.friendlies[i] = DestroyerStats;
        currentMap.friendlies[i].wPos = RandomWorldPointNoIsland();
        currentMap.friendlies[i].angle = R01() * 7;
        currentMap.friendlies->team = true;
        memcpy(currentMap.friendlies[i].batteries, DestroyerLoadout, sizeof(DestroyerLoadout)); 
        currentMap.friendlies[i].batteryCount = 3;//REMEMBER TO RESET!
        if(i == 0){defaultShip = currentMap.friendlies[i];}
    }   

    currentMap.ecount= MAX_SHIPS;
    for(int i = 0; i < currentMap.ecount; i++){
        currentMap.enemies[i] = DestroyerStats;
        currentMap.enemies[i].wPos = RandomWorldPointNoIsland();
        currentMap.enemies[i].angle = R01() * 7;
        currentMap.enemies->team = false;
        memcpy(currentMap.enemies[i].batteries, DestroyerLoadout, sizeof(DestroyerLoadout)); 
        currentMap.enemies[i].batteryCount = 3; //REMEMBER TO RESET!
    } 
}


void InitMapScene(){

    // worldScale = 1;
    // cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});
    timeScale = 0.1;

    //setup Ship CONSTANTS (overwrite from battlescene)
    int resLoc = GetShaderLocation(ship_frag, "multiplier");   
    int multiplier = 80;
    SetShaderValue(ship_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    resLoc = GetShaderLocation(ship_frag, "dotsize");   
    float dotsize = 0.3;
    SetShaderValue(ship_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);
}

void MapInputLoop(){

    if(IsMouseButtonDown(0)){


        // Island *click = WhatIslandIsThis(mousePos);
        // if(click){
        //     click->relativePosition = mousePos;
        //     RecalculateEdges2(click);
        //     printf("dragging %f, %f\n", mousePos.x, mousePos.y);
        // }

        // printf("click \n");
        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < currentMap.fcount; i++){
                currentMap.friendlies[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(ScreenToWorld(GetMousePosition()))){
            DrawCircleV(GetMousePosition(), 5, RED);
        }else{
            DrawCircleV(GetMousePosition(), 5, GREEN);
        }

        for(int i = 0; i < currentMap.fcount; i++){
           if(Vector2Distance(currentMap.friendlies[i].wPos, mousePos) < 0.3){
                currentMap.friendlies[i].selected = true;
            }
        }   
    }
    if(IsMouseButtonDown(1)){
        // printf("rclick \n");
        for(int i = 0; i < currentMap.fcount; i++){
            if(currentMap.friendlies[i].selected){
                currentMap.friendlies[i].moveTargetPosition = mousePos;
                currentMap.friendlies[i].hasMoveTarget = true;
            }
        }   
    }
    if(IsKeyPressed(KEY_SPACE)){
        RunRoutine("TimeRoutine");
    }
    if(IsKeyPressed(KEY_F)){

        bool run = RunRoutine("FocusRoutine");

        if(run){
            startingCameraPos = cameraPosition;
            startingZoom = worldScale;
            focusTarget = ScreenToWorld(GetMousePosition());
            RunRoutine("SwitchToBattleRoutine");
            if(startingZoom < 0.5){
                endZoom = 2;
            }else{
                endZoom = 0.4;
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
    // if(IsKeyDown(KEY_D)){
    //     cameraPosition.x += fixedDeltaTime * worldScale;
    // }
    // if(IsKeyDown(KEY_A)){
    //     cameraPosition.x -= fixedDeltaTime * worldScale;
    // }
    //  if(IsKeyDown(KEY_W)){
    //     cameraPosition.y += fixedDeltaTime * worldScale;
    // }
    // if(IsKeyDown(KEY_S)){
    //     cameraPosition.y -= fixedDeltaTime * worldScale;
    // }

    if(IsKeyPressed(KEY_R)){
        RandomizeMap();
    }
}

void MapFrameLoop(){

    int grey = 10;
    ClearBackground((Color){ grey, grey, grey, 255 });

    float gridSize = 1;
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
    PrepOceanPass(mousePos_ScreenCoords, 90, 0.08);
    EndOceanPass(); //flush buffer

    for(int d = 0; d < currentMap.ecount; d++){
        currentMap.enemies[d].isVisible = false;
    }

    PrepShipRangePass();
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive)continue;
        DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < currentMap.ecount; d++){
            if(!currentMap.enemies[d].alive)continue;
            if(Vector2Distance(currentMap.friendlies[i].wPos, currentMap.enemies[d].wPos) < SHIP_SEARCHRANGE){
                currentMap.enemies[d].isVisible = true;
            }
        }
    }
    EndOceanPass();

    //Set color red
    int resLoc = GetShaderLocation(ship_frag, "dotcolor");   
    Vector3 col = (Vector3){1, 0, 0};
    SetShaderValue(ship_frag, resLoc, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int d = 0; d < currentMap.ecount; d++){
        if(currentMap.enemies[d].isVisible && currentMap.enemies[d].alive){
            RenderShip(&currentMap.enemies[d], 0.7);
        }
    }
    EndShaderMode();

    //Set color white
    col = (Vector3){1, 1, 1};
    SetShaderValue(ship_frag, resLoc, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive)continue;
        RenderShip(&currentMap.friendlies[i], 1);
        SteerShip(&currentMap.friendlies[i], 1, true, currentMap.islands);
    }
    EndShaderMode();

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < currentMap.islandLength; i++){
        Render(&currentMap.islands[i]);
    }
    EndShaderMode();
}

