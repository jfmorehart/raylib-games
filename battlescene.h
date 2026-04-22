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

#include <alloca.h>
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern Island island[ISLANDCOUNT];

extern int shipCount;
extern Ship ships[MAX_SHIPS];

void InitBattleScene(){

    timeScale = 1;
    //cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});
    //timeScale = 0.1;
    //srand(time(NULL));
    //ShaderInit();
}

void BattleFrameLoop(){
    ClearBackground((Color){ 1, 1, 1, 255 });

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);
    mousePos_ScreenCoords.y = HEIGHT - mousePos_ScreenCoords.y;
    mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 2); 

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_ScreenCoords, 100, 0.02);
    EndOceanPass(); //flush buffer

    float gridSize = 0.2;
    for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}), GRAY);
    }
    for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), GRAY);
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
}