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
#include "rlgl.h"
#include "taskforce.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

Map mapFromDisk;

extern Battery DestroyerLoadout[SHIP_MAXBATTERIES];

extern DotShader oceanShader;
extern DotShader generalShader;
extern DotShader islandShader;
extern DotShader map_islandShader;

Ship destroyerShip;
// Ship battleShip;

bool focusing = false;

Vector2 startingCameraPos;
float startingZoom;
float endZoom;
Vector2 focusTarget;
bool isZoomed;

#pragma region routine



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

void SwitchToBattleRoutine(Routine * routine){
    float runtime = (unscaledTime - routine->startTime);
    if(runtime >= routine->duration){
        routine->isActive= false;
        focusing = false;
        SwitchScenes(Battle);
    }
}
void CallFocus(Vector2 wpos){

    bool run = RunRoutine("FocusRoutine");

    if(run){
        startingCameraPos = cameraPosition;
        startingZoom = worldScale;
        focusTarget = wpos;
        focusing = true;
        RunRoutine("SwitchToBattleRoutine");
        endZoom = 0.4;


        //place ships back into worldspace

        for(int i = 0 ; i < taskForceCount; i++){

            Vector2 delta = Vector2Subtract(tfs[i].destination, tfs[i].position);
            delta = Vector2Normalize(delta);

            for(int s = 0; s < tfs[i].shipCount; s++){
                if(tfs[i].ships[s]){
                    tfs[i].ships[s]->wPos = Vector2Add(tfs[i].position, tfs[i].ships[s]->wPos);

                    tfs[i].ships[s]->moveTargetPosition = tfs[i].destination;
                    tfs[i].ships[s]->hasMoveTarget = true;

                    tfs[i].ships[s]->angle = atan2f(delta.y, delta.x);
                }
            }
        }
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
#pragma endregion

#pragma region init
void RandomizeMap(){

    MakeLoadouts();

    // srand(time(NULL));
    for(int i = 0; i < ISLANDCOUNT; i++){
        mapFromDisk.islands[i] = CreateIsland(); 
    }
    mapFromDisk.islandLength  = ISLANDCOUNT;
    
    mapFromDisk.fcount = MAX_SHIPS;
    for(int i = 0; i < mapFromDisk.fcount; i++){

        mapFromDisk.friendlies[i] = DestroyerStats;
        mapFromDisk.friendlies[i].wPos = RandomWorldPointNoIsland();
        mapFromDisk.friendlies[i].angle = R01() * 7;
        mapFromDisk.friendlies->team = true;
        memcpy(mapFromDisk.friendlies[i].batteries, DestroyerLoadout, sizeof(DestroyerLoadout));
        InitRvecs(&mapFromDisk.friendlies[i]);
        if(i == 0){destroyerShip = mapFromDisk.friendlies[i];}
    }   

}


void InitMapScene(){

    // worldScale = 2;
    // cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});
    timeScale = 0.1;
    DotShaderValues(&generalShader, 0.3, 80, (Vector3){1, 1, 1});
    //setup Ship CONSTANTS (overwrite from battlescene)
    // int resLoc = GetShaderLocation(ship_frag, "multiplier");   
    // int multiplier = 80;
    // SetShaderValue(ship_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    // resLoc = GetShaderLocation(ship_frag, "dotsize");   
    // float dotsize = 0.3;
    // SetShaderValue(ship_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    taskForceCount = 0;
    for(int i =0 ; i < mapFromDisk.fcount; i++){

        bool found = false;
        if(!mapFromDisk.friendlies[i].alive)continue;
        
        for(int t = 0; t < taskForceCount; t++){ 
            if(tfs[t].shipCount + 1 >= MAX_SHIPS_IN_TF) continue;
            if(tfs[t].team != mapFromDisk.friendlies[i].team) continue;
            if(Vector2Distance(mapFromDisk.friendlies[i].wPos, tfs[t].position) < TF_MAX_RADIUS){
                tfs[t].ships[tfs[t].shipCount] = &mapFromDisk.friendlies[i];

                //wPos stores offset from tf center in transit
                tfs[t].ships[tfs[t].shipCount]->wPos = Vector2Subtract(tfs[t].position, tfs[t].ships[tfs[t].shipCount]->wPos);
                tfs[t].shipCount++;
                found = true;
            }   
        }
        if(found) continue;
        //TRIED ALL TFS AND DIDNT FIT INTO ANY!
        //make new!

        tfs[taskForceCount] = (TaskForce){0};
        tfs[taskForceCount].shipCount = 0;
        tfs[taskForceCount].min_speed = 0.09;
        tfs[taskForceCount].team = mapFromDisk.friendlies[i].team;
        tfs[taskForceCount].position = mapFromDisk.friendlies[i].wPos;
        tfs[taskForceCount].ships[tfs[taskForceCount].shipCount] = &mapFromDisk.friendlies[i];
        tfs[taskForceCount].ships[tfs[taskForceCount].shipCount]->wPos = Vector2Zero(); // in transit, wPos becomes offset from tf center
        tfs[taskForceCount].shipCount++;
        taskForceCount++;

        // printf("spawning new taskforce- ship - %f, %f", currentMap.friendlies[i].wPos.x,  tfs[taskForceCount].position.x);
    }

    for(int i =0 ; i < mapFromDisk.ecount; i++){

        bool found = false;
        for(int t = 0; t < taskForceCount; t++){ 
            if(tfs[t].shipCount + 1 >= MAX_SHIPS_IN_TF) continue;
            if(tfs[t].team != mapFromDisk.enemies[i].team) continue;
            if(Vector2Distance(mapFromDisk.enemies[i].wPos, tfs[t].position) < TF_MAX_RADIUS){
                tfs[t].ships[tfs[t].shipCount] = &mapFromDisk.enemies[i];

                //wPos stores offset from tf center in transit
                tfs[t].ships[tfs[t].shipCount]->wPos = Vector2Subtract(tfs[t].position, tfs[t].ships[tfs[t].shipCount]->wPos);
                tfs[t].shipCount++;
                found = true;
            }   
        }
        if(found) continue;
        //TRIED ALL TFS AND DIDNT FIT INTO ANY!
        //make new!

        tfs[taskForceCount].shipCount = 0;
        tfs[taskForceCount] = (TaskForce){0};
        tfs[taskForceCount].min_speed = 0.09;
        tfs[taskForceCount].team = false;
        tfs[taskForceCount].position = mapFromDisk.enemies[i].wPos;
        tfs[taskForceCount].ships[tfs[taskForceCount].shipCount] = &mapFromDisk.enemies[i];
        tfs[taskForceCount].ships[tfs[taskForceCount].shipCount]->wPos = Vector2Zero(); // in transit, wPos becomes offset from tf center
        tfs[taskForceCount].shipCount++;
        tfs[taskForceCount].destination = RandomWorldPointNoIsland();
        taskForceCount++;
    }

}
#pragma endregion

#pragma region loop
void MapInputLoop(){
    if(focusing) return;
    if(IsMouseButtonDown(0)){

        // printf("click \n");
        if(!IsKeyDown(KEY_LEFT_SHIFT)){
            for(int i = 0; i < mapFromDisk.fcount; i++){
                mapFromDisk.friendlies[i].selected = false;
            } 
        }

        if(IsPointWithinIslands(mousePos)){
            DrawCircleV(mousePos_ScreenCoords, 5, RED);
        }else{
            DrawCircleV(mousePos_ScreenCoords, 5, GREEN);
        }

        for(int i = 0; i < mapFromDisk.fcount; i++){
           if(Vector2Distance(mapFromDisk.friendlies[i].wPos, mousePos) < 0.3){
                mapFromDisk.friendlies[i].selected = true;
            }
        }   
    }
    if(IsMouseButtonDown(1)){
        // printf("rclick \n");
        for(int i = 0; i < mapFromDisk.fcount; i++){
            if(mapFromDisk.friendlies[i].selected){
                mapFromDisk.friendlies[i].moveTargetPosition = mousePos;
                mapFromDisk.friendlies[i].hasMoveTarget = true;
            }
        }   
    }
    if(IsKeyPressed(KEY_SPACE)){
        RunRoutine("TimeRoutine");
    }
    if(IsKeyPressed(KEY_F)){
        CallFocus(mousePos);
    }

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

    // float gridSize = 0.1;
    // grey = 20;
    // for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}),  CLITERAL(Color) { grey, grey, grey, 255 } );
    // }
    // for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
    //     DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), CLITERAL(Color) { grey, grey, grey, 255 });
    // }

    // Vector2 mGridPos = GridSnappedVector(mousePos, gridSize);
    // DrawCircleV(WorldToScreen(mGridPos), 3, WHITE);


    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_fragCoords, 90, 0.1);
    EndOceanPass(); //flush buffer

    for(int d = 0; d < mapFromDisk.ecount; d++){
        mapFromDisk.enemies[d].isVisible = false;
    }

    PrepShipRangePass();

    // for(int i = 0; i < currentMap.fcount; i++){
    //     if(!currentMap.friendlies[i].alive)continue;
    //     DrawCircleV(WorldToScreen(currentMap.friendlies[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

    //     for(int d = 0; d < currentMap.ecount; d++){
    //         if(!currentMap.enemies[d].alive)continue;
    //         if(Vector2Distance(currentMap.friendlies[i].wPos, currentMap.enemies[d].wPos) < SHIP_SEARCHRANGE){
    //             currentMap.enemies[d].isVisible = true;
    //             if(!focusing){
    //                 CallFocus(currentMap.friendlies[i].wPos);
    //             }
    //         }
    //     }
    // }

    printf("tf count: %d\n", taskForceCount);
    for(int i = 0; i < taskForceCount; i++){
        // if(tfs[i].shipCount <= 0) continue;
        if(tfs[i].team == false) continue;
        DrawCircleV(WorldToScreen(tfs[i].position), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < taskForceCount; d++){
            if(tfs[d].team == true) continue;
            // if(tfs[d].shipCount <= 0) continue;
            if(Vector2Distance(tfs[i].position, tfs[d].position) < SHIP_SEARCHRANGE){
                if(!focusing){
                    CallFocus(tfs[i].position);
                }
            }
        }
    }

    EndOceanPass();

    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES);   
    // //Set color red
    // Vector3 col = (Vector3){1, 0, 0};
    // SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);
    // BeginShaderMode(generalShader.shader);
    // for(int d = 0; d < currentMap.ecount; d++){
    //     if(currentMap.enemies[d].isVisible && currentMap.enemies[d].alive){
    //         RenderShip(&currentMap.enemies[d], 0.7);
    //         if(!focusing){
    //             SteerShip(&currentMap.enemies[d], true, currentMap.islands);
    //         }
    //     }
    // }
    // EndShaderMode();

    //Set color white
    // Vector3 col = (Vector3){1, 1, 1};
    // SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);
    // BeginShaderMode(generalShader.shader);
    // for(int i = 0; i < currentMap.fcount; i++){
    //     if(!currentMap.friendlies[i].alive)continue;
    //     RenderShip(&currentMap.friendlies[i], 1);
    //     if(!focusing){
    //         SteerShip(&currentMap.friendlies[i], true, currentMap.islands);
    //     }
    // }
    rlEnd();
    rlSetTexture(0); 
    // EndShaderMode();

    //islands 
    DotShaderValues(&map_islandShader, 0.04, 60, (Vector3){1, 1, 1});
    BeginShaderMode(map_islandShader.shader);
    
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES); 
    for(int i = 0; i < mapFromDisk.islandLength; i++){
        Render(&mapFromDisk.islands[i]);
    }
    rlEnd();
    rlSetTexture(0); 
    EndShaderMode();

    // Hit h = IntersectIslandsAndShips(Vector2Zero(), mousePos, &currentMap, 0.3);
    // if(h.hit){
    //     DrawLineEx(WorldToScreen(Vector2Zero()), WorldToScreen(h.hitPosition), 5, RED);
    // }else{
    //     DrawLineEx(WorldToScreen(Vector2Zero()), WorldToScreen(mousePos), 5, GREEN); 
    // }
    // DrawBeam(Vector2Zero(), mousePos, PI * 0.2, 10, 1, &currentMap, 0.3);

    for(int i = 0; i < taskForceCount; i++){
       Vector2 tfpos = WorldToScreen(tfs[i].position);

        if(tfs[i].team){
            if(IsMouseButtonDown(0)){
                if(Vector2DistanceSqr(tfpos, mousePos_ScreenCoords) < 50){
                    tfs[i].selected = true;
                }else{
                    tfs[i].selected = false;
                }
            }
            if(IsMouseButtonDown(1)){
                if(tfs[i].selected){
                    tfs[i].destination = mousePos;
                }
            }
        }


        if(!Vector2Equals(Vector2Zero(), tfs[i].destination)){
            Vector2 delta = Vector2Subtract(tfs[i].destination, tfs[i].position);
            if(Vector2LengthSqr(delta) < 0.01) {
                tfs[i].destination = Vector2Zero();
                continue;
            }
            delta = Vector2Scale(Vector2Normalize(delta), tfs[i].min_speed * scaledDeltaTime);
            if(!focusing){
                tfs[i].position = Vector2Add(tfs[i].position, delta);
            }

        }

        if(tfs[i].team){
            if(tfs[i].selected){
                DrawCircle(WorldToScreen(tfs[i].position).x, WorldToScreen(tfs[i].position).y, 3, BLUE); 
            }else{
                DrawCircle(WorldToScreen(tfs[i].position).x, WorldToScreen(tfs[i].position).y, 3, GRAY); 
            }
            DrawText("Task Force 32", tfpos.x - 30, tfpos.y - 20, 1, WHITE);
        }else{
            if(IsKeyPressed(KEY_S)){
                DrawCircle(WorldToScreen(tfs[i].position).x, WorldToScreen(tfs[i].position).y, 3, RED); 
                DrawText("OPFOR", tfpos.x - 30, tfpos.y - 20, 1, RED);
            }
        }
    }
}

void MapUIRender(){
    float diff = (WIDTH - HEIGHT) * 0.4;
    int border = 30;
    float dayscaler = 470;
    float sct = worldTime * dayscaler;  
    float td = sct / (60.00 * 24.00);
    int days = floorf(td);
    int hrs = (td - days) * 24;
    const char * str = TextFormat("%ddays, %dhrs", days, hrs);
    DrawText(str, diff + border, border - 10, 12, GRAY);
}
#pragma endregion
