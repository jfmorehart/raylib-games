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

#include "rlgl.h"


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

extern DotShader islandShader;
extern DotShader oceanShader;
extern DotShader shipShader;
extern DotShader explosionShader;
extern DotShader lightShader;

BeamHits allHits[MAX_SHIPS * SHIP_MAXBATTERIES];
int friendlyBeams = 0;
int enemyBeams= 0;

void InitBattleScene(){

    timeScale = 1;
    //setup Ship CONSTANTS (overwrite from mapscene)
    DotShaderValues(&shipShader,0.2, 120, (Vector3){1, 1, 1});

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
    int grey = 1;
    ClearBackground((Color){ grey, grey,grey * 3, 255 });
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

    for(int d = 0; d < currentMap.ecount; d++){
        currentMap.friendlies[d].isVisible = false;
    }


    //DRAW BEAMS
    friendlyBeams= 0;
    enemyBeams = 0;

    Vector3 col = (Vector3){1, 1, 1};
    // DotShaderValues(&shipShader, 0.2, 230, col);
    BeginShaderMode(lightShader.shader);
    rlSetTexture(rlGetTextureIdDefault());   
    BeginBlendMode(BLEND_ADDITIVE);                                                                              
    rlBegin(RL_TRIANGLES);
    for(int i = 0; i < currentMap.fcount; i++){
        for(int j = 0; j < currentMap.friendlies[i].batteryCount; j++){
            if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene){
                allHits[i * SHIP_MAXBATTERIES + j] = (BeamHits){0};
                continue;
            }
            if(friendlyBeams < 1 || (friendlyBeams < 5 && RVec_Perlin(currentMap.friendlies[i].batteries[j]._r_index, 0.01).x > 0.5)){
                allHits[i * SHIP_MAXBATTERIES + j] = RenderBatteryBeam(&currentMap.friendlies[i].batteries[j], &currentMap.friendlies[i]);
                if( allHits[i * SHIP_MAXBATTERIES + j].hitcount > 0){
                    friendlyBeams++;
                }
            }
        }
    }
    for(int i = 0; i < currentMap.ecount; i++){
        if(!currentMap.enemies[i].alive || !currentMap.enemies[i].includedInScene)continue;
        for(int j = 0; j < currentMap.enemies[i].batteryCount; j++){
            if(enemyBeams < 1 || (enemyBeams < 5 && RVec_Perlin(currentMap.enemies[i].batteries[j]._r_index, 0.01).x > 0.2)){
                BeamHits bh = RenderBatteryBeam(&currentMap.enemies[i].batteries[j], &currentMap.enemies[i]);
                if(bh.hitcount > 0){
                    enemyBeams++;
                }
            }
        }
    }
    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();


    //DRAW HIGHLIGHTS
    col = (Vector3){0.8, 0, 0};
    DotShaderValues(&shipShader,0.2, 230, col);
    BeginShaderMode(shipShader.shader);
    for(int i = 0; i < SHIP_MAXBATTERIES * MAX_SHIPS; i++){
        BeamHits beamHits = allHits[i];
        if(beamHits.hitcount < 1) continue;
        //find local min and draw some shit
        int localMinIndex = -1;
        float closest = 999;
        for(int h = 0; h < beamHits.hitcount; h++){
            // if(!beamHits.hits[h].hit)continue;
            float tdist = Vector2Distance(beamHits.origin, beamHits.hits[h].hitPosition);
            if(tdist < closest){
                closest = tdist;
                localMinIndex = h;
            }
            printf("%d -- %d -- dist %f -- %d \n", i, h, tdist, (h == localMinIndex));
        }
        if(localMinIndex == -1){
            printf("failed to find localmin - %d\n", i);
            continue;
        }
        printf("drawing local min");
        DrawCircleV(WorldToScreen(beamHits.hits[localMinIndex].hitPosition), 2, GRAY);

        Vector2 v1 = beamHits.hits[localMinIndex].hitPosition;
        //a little past localmin
        Vector2 delta = Vector2Subtract(beamHits.hits[localMinIndex].hitPosition, beamHits.origin);
        Vector2 v3 = Vector2Add(v1, Vector2Scale(Vector2Normalize(delta), 0.01));
        //otherwise, draw the lines
        if(localMinIndex > 0){
            Vector2 v2 = beamHits.hits[localMinIndex - 1].hitPosition;
            v2 = Vector2Add(v1, Vector2Scale(Vector2Normalize(Vector2Subtract(v2, v1)), 0.01));
            DrawTriangle(WorldToScreen(v1), WorldToScreen(v2), WorldToScreen(v3), WHITE);
        }
        if(localMinIndex < beamHits.hitcount - 1){
            Vector2 v2 = beamHits.hits[localMinIndex + 1].hitPosition;
            v2 = Vector2Add(v1, Vector2Scale(Vector2Normalize(Vector2Subtract(v2, v1)), 0.01));
            DrawTriangle(WorldToScreen(v1), WorldToScreen(v3), WorldToScreen(v2), WHITE);
        }
    }
    EndShaderMode();
    EndBlendMode();

    PrepShipRangePass();
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), WorldToPixels(SHIP_SEARCHRANGE * 0.5), WHITE);

        for(int d = 0; d < currentMap.ecount; d++){
            if(!currentMap.enemies[d].alive || !currentMap.enemies[d].includedInScene)continue;
            if(Vector2Distance(currentMap.friendlies[i].wPos, currentMap.enemies[d].wPos) < SHIP_SEARCHRANGE){
                currentMap.enemies[d].isVisible = true;
                currentMap.friendlies[i].isVisible = true;
            }
        }
    }
    EndShaderMode();

    col = (Vector3){0.5, 0.5, 0.5};
    DotShaderValues(&shipShader,0.2, 230, col);
    BeginShaderMode(shipShader.shader);
    for(int d = 0; d < currentMap.ecount; d++){
        if(currentMap.enemies[d].isVisible && currentMap.enemies[d].alive && currentMap.enemies[d].includedInScene){
            // RenderShip(&currentMap.enemies[d], 0.3);
            SteerShipBattle(&currentMap.enemies[d], true, currentMap.islands);
        }
    }
    EndShaderMode();

    //Set color white
    col = (Vector3){1, 1, 1};
    DotShaderValues(&shipShader,0.2, 230, col);
    BeginShaderMode(shipShader.shader);
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        RenderShip(&currentMap.friendlies[i], 0.3);
        SteerShipBattle(&currentMap.friendlies[i], false, currentMap.islands);

        // DrawLineEx(WorldToScreen(currentMap.friendlies[i].wPos), mousePos_ScreenCoords, 3,  WHITE);
        // DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), 30, WHITE);
    }
    EndShaderMode();


    //trace ship linesx
    col = (Vector3){0.1, 0.1, 0.1};
    DotShaderValues(&shipShader,0.1, 230, col);
    BeginShaderMode(shipShader.shader);
    for(int i = 0; i < currentMap.fcount; i++){
        if(!currentMap.friendlies[i].alive || !currentMap.friendlies[i].includedInScene)continue;
        ShipCombat(&currentMap.friendlies[i], currentMap.enemies, currentMap.ecount);
    }
    for(int i = 0; i < currentMap.ecount; i++){
        if(!currentMap.enemies[i].alive || !currentMap.enemies[i].includedInScene)continue;
        ShipCombat(&currentMap.enemies[i], currentMap.friendlies, currentMap.fcount);
    }
    EndShaderMode();

    //beaches
    DotShaderValues(&islandShader,0.3, 230, col);
    BeginShaderMode(islandShader.shader);
    for(int i = 0; i < currentMap.islandLength; i++){
        RenderBeaches(&currentMap.islands[i]);
    }
    EndShaderMode();

    DotShaderValues(&islandShader,0.08, 230, col);
    BeginShaderMode(islandShader.shader);
    for(int i = 0; i < currentMap.islandLength; i++){
        Render(&currentMap.islands[i]);
    }
    EndShaderMode();

   
    //BULLETS
    col = (Vector3){0.5, 0.5, 0.5};
    DotShaderValues(&shipShader,0.15, 230, col);
    BeginShaderMode(shipShader.shader);
    UpdateAndRenderBullets(bulletPool, bulletCount, allShipsIncludedInScene, allShipsIncludedCount);
    EndShaderMode();

    //Explosions!
    col = (Vector3){1, 1, 1};
    SetShaderValue(explosionShader.shader, explosionShader.dloc, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(explosionShader.shader);
    UpdateAndRenderBlobs(smokePool, smokeCount);
    EndShaderMode();
    EndShaderMode();
    
    //Splashes
    col = (Vector3){0.2, 0.2, 0.2};
    DotShaderValues(&shipShader,0.3, 180, col);
    BeginShaderMode(shipShader.shader);
    UpdateAndRenderBlobs(splashPool, splashCount);
    EndShaderMode();

    // BeginBlendMode(BLEND_ADDITIVE);  
    // EndBlendMode();

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
            if(IsPointInShip(mousePos, &currentMap.friendlies[i], 0.3)){
                DrawCircleV(mousePos_ScreenCoords, 5, RED);
            }
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
    // Hit h = IntersectIslandsAndShips(Vector2Zero(), mousePos, &currentMap, 0.3);
    // if(h.hit){
    //     DrawLineEx(WorldToScreen(Vector2Zero()), WorldToScreen(h.hitPosition), 5, RED);
    // }else{
    //     DrawLineEx(WorldToScreen(Vector2Zero()), WorldToScreen(mousePos), 5, GREEN); 
    // }
    
}

void BattleUIRender(){

    float diff = (WIDTH - HEIGHT) * 0.4;
    int border = 30;
    float dayscaler = 470;
    float sct = worldTime * dayscaler;  
    float td = sct / (60.00 * 24.00);
    int days = floorf(td);
    float ht = (td - days) * 24;
    int hrs = floorf(ht);
    int mins = (ht - hrs) * 60;
    const char * str = TextFormat("%ddays, %dhrs, %dmins", days, hrs, mins);
    DrawText(str, diff + border, border - 10, 12, GRAY);
    DrawText("Battle Off Cape Esperance",diff + border + 250, border - 10, 12, GRAY);
}