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

extern Map mapFromDisk;

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
extern DotShader generalShader;
extern DotShader explosionShader;
extern DotShader lightShader;
extern DotShader illuminatedShader;

void InitBattleScene(){

    printf("cpos %f, %f, wscale %f\n", cameraPosition.x, cameraPosition.y, worldScale);
    worldScale = 0.25;
    timeScale = 1;
    //setup Ship CONSTANTS (overwrite from mapscene)
    DotShaderValues(&generalShader,0.2, 120, (Vector3){1, 1, 1});

    //determine which ships are included in the scene
    allShipsIncludedCount = 0;
    int fc = 0;
    int ec = 0;
    printf("disk values: fcount %d, ecount %d\n", mapFromDisk.fcount, mapFromDisk.ecount);

    for(int i = 0; i < mapFromDisk.fcount; i++){
        BattleSceneIntroReset(&mapFromDisk.friendlies[i]);
        mapFromDisk.friendlies[i].includedInScene = false;
        if(!mapFromDisk.friendlies[i].alive) continue;
        printf("mapFromDisk.friendlies[%d].wPos) = %f, %f \n", i, mapFromDisk.friendlies[i].wPos.x, mapFromDisk.friendlies[i].wPos.y);
        printf("w2s %f %f\n", WorldToScreen(mapFromDisk.friendlies[i].wPos).x,  WorldToScreen(mapFromDisk.friendlies[i].wPos).y);
        if(IsOnScreen(mapFromDisk.friendlies[i].wPos)){
            mapFromDisk.friendlies[i].includedInScene = true;
            allShipsIncludedInScene[allShipsIncludedCount] = &mapFromDisk.friendlies[i];
            allShipsIncludedCount++;
            fc++;
        }
    }
    for(int i = 0; i < mapFromDisk.ecount; i++){
        BattleSceneIntroReset(&mapFromDisk.enemies[i]);
        mapFromDisk.enemies[i].includedInScene = false;
        if(!mapFromDisk.enemies[i].alive)continue;
        if(IsOnScreen(mapFromDisk.enemies[i].wPos)){
            mapFromDisk.enemies[i].includedInScene = true;
            allShipsIncludedInScene[allShipsIncludedCount] = &mapFromDisk.enemies[i];
            allShipsIncludedCount++;
            ec++;
        }
    }
    printf("rendering these ships: friendly %d, enemy%d\n", fc, ec);
}

void BattleFrameLoop(){
    int grey = 1;
    ClearBackground((Color){ grey, grey,grey * 3, 255 });
    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_fragCoords, 100, 0.02);
    
    EndOceanPass(); //flush buffer

    for(int d = 0; d < mapFromDisk.ecount; d++){
        // currentMap.enemies[d].isVisible = false;
        // currentMap.enemies[d].illuminationThisFrame -= scaledDeltaTime * 5;
        mapFromDisk.enemies[d].illuminationThisFrame = 0;//fmaxf(currentMap.enemies[d].illuminationThisFrame, 0);
    }

    for(int d = 0; d < mapFromDisk.ecount; d++){
        mapFromDisk.friendlies[d].isVisible = false;
    }

    //DRAW BEAMS
    Vector3 col = (Vector3){1, 1, 1};
    // DotShaderValues(&shipShader, 0.2, 230, col);
    BeginShaderMode(lightShader.shader);
    rlSetTexture(rlGetTextureIdDefault());   
    BeginBlendMode(BLEND_ADDITIVE);                                                                              
    rlBegin(RL_TRIANGLES);
    for(int i = 0; i < mapFromDisk.fcount; i++){
        for(int j = 0; j < mapFromDisk.friendlies[i].batteryCount; j++){
            RenderBatteryBeam(&mapFromDisk.friendlies[i].batteries[j], &mapFromDisk.friendlies[i]);
        }
    }
    int liveEnemies = 0;
    for(int i = 0; i < mapFromDisk.ecount; i++){
        if(!mapFromDisk.enemies[i].alive || !mapFromDisk.enemies[i].includedInScene)continue;
        liveEnemies++;
        for(int j = 0; j < mapFromDisk.enemies[i].batteryCount; j++){
            RenderBatteryBeam(&mapFromDisk.enemies[i].batteries[j], &mapFromDisk.enemies[i]);
        }
    }
    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();
    EndBlendMode();
    if(liveEnemies < 1){
        char fullstring[30] = "livemaps/";
        strcat(fullstring, mapFromDisk.filename);
        FILE *fptr = fopen(fullstring, "wb");
        if(fptr){
            // Write some text to the file
            fwrite(&mapFromDisk, sizeof(Map), 1, fptr);   
            // Close the file
            fclose(fptr); 
            printf("saved map to livemaps folder");
        }
        // fread(&mapFromDisk, sizeof(Map),1);
        WonBattleSwitch();
    }

    PrepShipRangePass();
    for(int i = 0; i < mapFromDisk.fcount; i++){
        if(!mapFromDisk.friendlies[i].alive || !mapFromDisk.friendlies[i].includedInScene)continue;
        DrawCircleV(WorldToScreen(mapFromDisk.friendlies[i].wPos), WorldToPixels(SHIP_SEARCHRANGE * 0.5), WHITE);

        for(int d = 0; d < mapFromDisk.ecount; d++){
            if(!mapFromDisk.enemies[d].alive || !mapFromDisk.enemies[d].includedInScene)continue;
            if(Vector2Distance(mapFromDisk.friendlies[i].wPos, mapFromDisk.enemies[d].wPos) < SHIP_SEARCHRANGE){
                mapFromDisk.enemies[d].isVisible = true;
                mapFromDisk.friendlies[i].isVisible = true;
            }
            else if (Vector2Distance(mapFromDisk.friendlies[i].wPos, mapFromDisk.enemies[d].wPos) < SHIP_SEARCHRANGE * 1.5){
                // currentMap.enemies[d].isVisible = true;
                mapFromDisk.enemies[d].illuminationThisFrame += 0.1 * scaledDeltaTime;
            }
        }
    }
    EndShaderMode();

    col = (Vector3){1, 0.5, 0.5};
    DotShaderValues(&illuminatedShader,0.2, 230, col);
    BeginShaderMode(illuminatedShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES);       
    for(int d = 0; d < mapFromDisk.ecount; d++){
        if(mapFromDisk.enemies[d].illuminationThisFrame > 0.1 && mapFromDisk.enemies[d].alive && mapFromDisk.enemies[d].includedInScene){
            RenderShipColor(&mapFromDisk.enemies[d], 0.3, Vector3Scale(col, fminf(1, mapFromDisk.enemies[d].illuminationThisFrame)));
            SteerShipBattle(&mapFromDisk.enemies[d], true, mapFromDisk.islands);
        }
    }
    rlEnd();          
    rlSetTexture(0);    
    EndShaderMode();

    //Set color white
    col = (Vector3){1, 1, 1};
    DotShaderValues(&illuminatedShader,0.2, 230, col);
    BeginShaderMode(illuminatedShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES);      
    int active_fcount = 0;
    for(int i = 0; i < mapFromDisk.fcount; i++){
        if(!mapFromDisk.friendlies[i].alive || !mapFromDisk.friendlies[i].includedInScene)continue;
        RenderShipColor(&mapFromDisk.friendlies[i], 0.3, col);
        SteerShipBattle(&mapFromDisk.friendlies[i], false, mapFromDisk.islands);
        active_fcount++;
        // DrawLineEx(WorldToScreen(currentMap.friendlies[i].wPos), mousePos_ScreenCoords, 3,  WHITE);
        // DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), 30, WHITE);
    }
    rlEnd();          
    rlSetTexture(0);    
    EndShaderMode();

    if(active_fcount < 1){
        //all friendlies lost
        LostBattleSwitch();
    }

    //trace ship lines
    col = (Vector3){0.1, 0.1, 0.1};
    DotShaderValues(&generalShader,0.1, 230, col);
    BeginShaderMode(generalShader.shader);
    for(int i = 0; i < mapFromDisk.fcount; i++){
        if(!mapFromDisk.friendlies[i].alive || !mapFromDisk.friendlies[i].includedInScene)continue;
        ShipCombat(&mapFromDisk.friendlies[i], mapFromDisk.enemies, mapFromDisk.ecount);
    }
    for(int i = 0; i < mapFromDisk.ecount; i++){
        if(!mapFromDisk.enemies[i].alive || !mapFromDisk.enemies[i].includedInScene)continue;
        ShipCombat(&mapFromDisk.enemies[i], mapFromDisk.friendlies, mapFromDisk.fcount);
    }
    EndShaderMode();

    // //beaches
    // DotShaderValues(&islandShader,0.3, 230, col);
    // BeginShaderMode(islandShader.shader);
    // for(int i = 0; i < currentMap.islandLength; i++){
    //     RenderBeaches(&currentMap.islands[i]);
    // }
    // EndShaderMode();

    col = (Vector3){1,1,1};
    DotShaderValues(&islandShader, 0.3, 230, col);
    BeginShaderMode(islandShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES);  
    for(int i = 0; i < mapFromDisk.islandLength; i++){
        Render(&mapFromDisk.islands[i]);
    }
    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();

    BeginBlendMode(BLEND_ADDITIVE);  
    //BULLETS
    col = (Vector3){0.3, 0.3, 0.3};
    DotShaderValues(&generalShader,0.10, 230, col);
    BeginShaderMode(generalShader.shader);
    UpdateAndRenderBullets(bulletPool, bulletCount, allShipsIncludedInScene, allShipsIncludedCount);
    EndShaderMode();

    EndBlendMode();
    //Explosions!
    col = (Vector3){1, 1, 1};
    SetShaderValue(explosionShader.shader, explosionShader.dloc, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(explosionShader.shader);
    UpdateAndRenderBlobs(smokePool, smokeCount);
    EndShaderMode();
    EndShaderMode();
    
    //Splashes
    col = (Vector3){0.2, 0.2, 0.2};
    DotShaderValues(&generalShader,0.3, 180, col);
    BeginShaderMode(generalShader.shader);
    UpdateAndRenderBlobs(splashPool, splashCount);
    EndShaderMode();

    if(IsMouseButtonDown(0)){

        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < mapFromDisk.fcount; i++){
                mapFromDisk.friendlies[i].selected = false;
                mapFromDisk.enemies[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(mousePos)){
            DrawCircleV(mousePos_ScreenCoords, 5, RED);
        }else{
            DrawCircleV(mousePos_ScreenCoords, 5, GREEN);
        }
        for(int i = 0; i < mapFromDisk.fcount; i++){
            if(IsPointInShip(mousePos, &mapFromDisk.friendlies[i], 0.3)){
                DrawCircleV(mousePos_ScreenCoords, 5, RED);
            }
        }

        for(int i = 0; i < mapFromDisk.fcount; i++){
            if(!mapFromDisk.friendlies[i].alive)continue;
            if(Vector2Distance(mapFromDisk.friendlies[i].wPos, mousePos) < 0.1){
                mapFromDisk.friendlies[i].selected = true;
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
        for(int i = 0; i < mapFromDisk.fcount; i++){
            if(mapFromDisk.friendlies[i].selected){
                mapFromDisk.friendlies[i].moveTargetPosition = mousePos;
                mapFromDisk.friendlies[i].hasMoveTarget = true;
            }
        }   
    }
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