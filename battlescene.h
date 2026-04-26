#pragma once
#include "mapscene.h"
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

#include <math.h>       
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

extern Island island[ISLANDCOUNT];

extern int shipCount;
extern Ship ships[MAX_SHIPS];

int bulletCham;
int bulletCount = 100;
Bullet bulletPool[100];

int smokeCham;
int smokeCount = 100;
Smoke smokePool[100];

int splashCham;
int splashCount = 100;
Smoke splashPool[100];

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
    for(int i = 0; i < shipCount; i++){
        if(!ships[i].alive)continue;
        ships[i].includedInScene = IsOnScreen(ships[i].wPos);
    }
    for(int i = 0; i < eshipCount; i++){
        if(eships[i].alive)continue;
        eships[i].includedInScene = IsOnScreen(eships[i].wPos);
    }
}

void BattleFrameLoop(){
    ClearBackground((Color){ 5, 5, 20, 255 });

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);
    mousePos_ScreenCoords.y = HEIGHT - mousePos_ScreenCoords.y;
    mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 2); 

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_ScreenCoords, 100, 0.02);
    EndOceanPass(); //flush buffer

    // float gridSize = 0.2;
    // for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}), GRAY);
    // }
    // for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), GRAY);
    // }


    for(int d = 0; d < eshipCount; d++){
        eships[d].isVisible = false;
    }

    PrepShipRangePass();
    for(int i = 0; i < shipCount; i++){
        if(!ships[i].alive || !ships[i].includedInScene)continue;
        DrawCircleV(WorldToScreen(ships[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < eshipCount; d++){
            if(!eships[d].alive || !eships[d].includedInScene)continue;
            if(Vector2Distance(ships[i].wPos, eships[d].wPos) < SHIP_SEARCHRANGE){
                eships[d].isVisible = true;
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
    for(int d = 0; d < eshipCount; d++){
        if(eships[d].isVisible && eships[d].alive && eships[d].includedInScene){
            RenderShip(&eships[d], 0.3);
        }
    }
    EndShaderMode();

    //Set color white
    col = (Vector3){1, 1, 1};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    for(int i = 0; i < shipCount; i++){
        if(!ships[i].alive || !ships[i].includedInScene)continue;
        RenderShip(&ships[i], 0.3);
        SteerShip(&ships[i], 0.05, false);
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
    for(int i = 0; i < shipCount; i++){
        if(!ships[i].alive || !ships[i].includedInScene)continue;
        ShipCombat(&ships[i], eships, eshipCount);
    }
    for(int i = 0; i < eshipCount; i++){
        if(!eships[i].alive || !eships[i].includedInScene)continue;
        ShipCombat(&eships[i], ships, shipCount);
    }
    EndShaderMode();

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < ISLANDCOUNT; i++){
        Render(&island[i]);
    }
    EndShaderMode();

        //Set color yellow

    mLoc = GetShaderLocation(ship_frag, "multiplier");   
    mult = 200;
    dotsize = 0.2;
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);
    col = (Vector3){0.7, 0.7, 0.7};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);

    BeginShaderMode(ship_frag);
    UpdateAndRenderBullets(bulletPool, bulletCount, eships, eshipCount);
    EndShaderMode();

    col = (Vector3){1, 0.8, 0};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    UpdateAndRenderBlobs(smokePool, smokeCount);
    EndShaderMode();

    dotsize = 0.3;
    SetShaderValue(ship_frag, dLoc, &dotsize, SHADER_UNIFORM_FLOAT);
    mult = 100;
    SetShaderValue(ship_frag, mLoc, &mult, SHADER_UNIFORM_INT);
    col = (Vector3){0.1, 0.1, 0.2};
    SetShaderValue(ship_frag, colorLocation, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(ship_frag);
    UpdateAndRenderBlobs(splashPool, splashCount);
    EndShaderMode();

    if(IsMouseButtonDown(0)){

        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < shipCount; i++){
                ships[i].selected = false;
                eships[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(ScreenToWorld(GetMousePosition()))){
            DrawCircleV(GetMousePosition(), 5, RED);
        }else{
            DrawCircleV(GetMousePosition(), 5, GREEN);
        }

        for(int i = 0; i < shipCount; i++){
            if(!ships[i].alive)continue;
            if(Vector2Distance(ships[i].wPos, mousePos) < 0.1){
                ships[i].selected = true;
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

    if(IsMouseButtonDown(1)){
        for(int i = 0; i < shipCount; i++){
            if(ships[i].selected){
                ships[i].moveTargetPosition = mousePos;
                ships[i].hasMoveTarget = true;
            }
        }   
    }
}