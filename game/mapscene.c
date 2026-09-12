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
#include "text.h"
#include "cutscene.h"
#include "mapscene.h"
#include "filesystem.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern TaskForce activeTFs [MAX_TFS];
extern int taskForceCount;

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

TextBuffer rightBar;

extern PolyPoly cruiser;

#pragma region routine

bool dayActive;


void TimeRoutine(Routine *routine){

    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    float dt = 1 - fabs(0.5 - pct) / 0.5;
    timeScale = 2 * dt + 0.5;
    dayActive = true;
    if(pct > 1){
        dayActive = false;
        routine->isActive= false;
        timeScale = 0.05;
        // printf("End Time Routine");
    }
}
void LostBattleSwitch(){
    SetCutscene(SinkingFriendly);
    SwitchScenes(CutScene);
}
void DisengageBattleSwitch(){
    SetCutscene(DisengageBattle);
    SwitchScenes(CutScene);
}
void WonBattleSwitch(){
    SetCutscene(SinkingEnemy);
    SwitchScenes(CutScene);
}

void SwitchToBattleRoutine(Routine * routine){
    float runtime = (unscaledTime - routine->startTime);
    if(runtime >= routine->duration){
        routine->isActive= false;
        focusing = false;
        SetCutscene(0);
        SwitchScenes(CutScene);
        // SwitchScenes(Battle);
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

            bool inbattle = Vector2Distance(activeTFs[i].position, focusTarget) < MAP_SEARCHRANGE + TF_MAX_RADIUS;

            Vector2 delta = Vector2Subtract(activeTFs[i].destination, activeTFs[i].position);
            delta = Vector2Normalize(delta);

            for(int s = 0; s < activeTFs[i].shipCount; s++){
                if(activeTFs[i].ships[s]){
                    if(inbattle){
                        activeTFs[i].ships[s]->includedInScene = true;
                    }else{
                        activeTFs[i].ships[s]->includedInScene = false;
                    }

                    activeTFs[i].ships[s]->wPos = Vector2Add(activeTFs[i].position, activeTFs[i].ships[s]->wPos);

                    activeTFs[i].ships[s]->moveTargetPosition = activeTFs[i].destination;
                    activeTFs[i].ships[s]->hasMoveTarget = true;

                    activeTFs[i].ships[s]->angle = atan2f(delta.y, delta.x);
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
        mapFromDisk.friendlies[i].team = true;
        memcpy(mapFromDisk.friendlies[i].batteries, DestroyerLoadout, sizeof(DestroyerLoadout));
        InitRvecs(&mapFromDisk.friendlies[i]);
        if(i == 0){destroyerShip = mapFromDisk.friendlies[i];}
    }   

}


void InitMapScene(){

    dayActive = false;
    ClearBuffer(&rightBar);
    AddBufferText(&rightBar, "LCLICK - Select\n");
    AddBufferText(&rightBar, "RCLICK - Order\n");
    AddBufferText(&rightBar, "P - Picket\n");
    AddBufferText(&rightBar, "SPACE - Next Day\n");
    AddBufferText(&rightBar, "LCLICK - Select\n");
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
    CreateTaskForcesFromMapFile(&mapFromDisk);
    TFShipsWorldToLocal();
    // cruiser.polyCenter = ScreenToWorld((Vector2){WIDTH * 1.1, HEIGHT * 0.5});
    // cruiser.polyScale = 0.4;
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

    if(IsKeyPressed(KEY_R)){
        RandomizeMap();
    }
}

float objUpdate_last;
float objUpdate_delay = 0.2;
void ObjectiveUpdate(){
    for(int i = 0; i < mapFromDisk.objective_count; i++){
        
        Objective * obj = &mapFromDisk.map_objectives[i];

        if(obj->type == Spotter){ obj->alive = false;} //disable all spotters at the start of each tick

        for(int e = 0; e < taskForceCount; e++){
            // Ship * en = &mapFromDisk.enemies[e];

            TaskForce * tf = &activeTFs[e];
            if(obj->team == tf->team) continue;
            if(Vector2Distance(obj->position, tf->position) <  MAP_SEARCHRANGE * 1.2){
                if(obj->type == Spotter){
                    obj->alive = true; //set them active if they spot something;
                }else if (obj->type == ReachTarget){
                    SwitchScenes(Menu);
                    return;
                }
            }
        }
    }
}

float spotterblink_last;
float spotterblink_delay = 0.6;
bool spotters_blink;

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

    for(int i = 0; i < taskForceCount; i++){
        // if(tfs[i].shipCount <= 0) continue;
        if(activeTFs[i].team == false) continue;
        DrawCircleV(WorldToScreen(activeTFs[i].position), WorldToPixels( MAP_SEARCHRANGE), WHITE);

        for(int d = 0; d < taskForceCount; d++){
            if(activeTFs[d].team == true) continue;
            if(activeTFs[d].shipCount <= 0) continue;
            if(Vector2Distance(activeTFs[i].position, activeTFs[d].position) <  MAP_SEARCHRANGE){
                if(!focusing){
                    CallFocus(activeTFs[i].position);
                }
            }
        }
    }

    EndOceanPass();

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


    //objectives
    for(int i = 0; i < mapFromDisk.objective_count; i++){
        if(mapFromDisk.map_objectives[i].type == ReachTarget){
            if(mapFromDisk.map_objectives[i].team){
                DrawCircleV(WorldToScreen(mapFromDisk.map_objectives[i].position), 8, YELLOW);
            }else{
                DrawCircleV(WorldToScreen(mapFromDisk.map_objectives[i].position), 8, GOLD);
            }
        }
        else if(mapFromDisk.map_objectives[i].type == Spotter){
            if(mapFromDisk.map_objectives[i].alive && spotters_blink) //set to alive when actively spotting smth
            {
                DrawCircleV(WorldToScreen(mapFromDisk.map_objectives[i].position), 2, RED);
            }
        }
    }
    if(scaledTime - objUpdate_last > objUpdate_delay){
        objUpdate_last = scaledTime;
        ObjectiveUpdate();
    }
    if(unscaledTime - spotterblink_last > spotterblink_delay){
        spotterblink_last = unscaledTime;
        spotters_blink = !spotters_blink;
    }



    for(int i = 0; i < taskForceCount; i++){
       Vector2 tfpos = WorldToScreen(activeTFs[i].position);

        if(activeTFs[i].team && !dayActive){

            if(IsMouseButtonDown(0)){
                if(Vector2DistanceSqr(tfpos, mousePos_ScreenCoords) < 50){
                    activeTFs[i].selected = true;
                }else{
                    activeTFs[i].selected = false;
                }
            }

            if(activeTFs[i].selected){
                Hit hit = AllIslandsIntersect(mapFromDisk.islands, (Edge){activeTFs[i].position, mousePos});
                if(hit.hit){
                    DrawLineEx(WorldToScreen(activeTFs[i].position),WorldToScreen(hit.hitPosition), 1, GREEN);
                    DrawLineEx(WorldToScreen(hit.hitPosition), WorldToScreen(mousePos), 1, RED);
                }else{
                    DrawLineEx(WorldToScreen(activeTFs[i].position), WorldToScreen(mousePos), 1, GREEN);
                }
                if(IsMouseButtonDown(1) && !hit.hit){
                    activeTFs[i].selected = false;
                    activeTFs[i].destination = mousePos;
                }
            }

        }

        bool validPath = false;
        if(!Vector2Equals(Vector2Zero(), activeTFs[i].destination)){
            Vector2 delta = Vector2Subtract(activeTFs[i].destination, activeTFs[i].position);
            if(Vector2LengthSqr(delta) < 0.01 && !dayActive) {
                if(activeTFs[i].team){
                    activeTFs[i].destination = Vector2Zero();
                    continue;
                }else{
                    activeTFs[i].destination = PickRandomLegalDestination(activeTFs[i].position);
                }

            }
            delta = Vector2Scale(Vector2Normalize(delta), activeTFs[i].min_speed * scaledDeltaTime);
            validPath = true;
            if(!focusing){
                activeTFs[i].position = Vector2Add(activeTFs[i].position, delta);
            }

        }

        if(activeTFs[i].team){
            if(activeTFs[i].selected){
                Vector2 pos = WorldToScreen(activeTFs[i].position);

                if(validPath){
                    Vector2 target = activeTFs[i].destination;
                    Vector2 delta = Vector2Normalize(Vector2Subtract( target, activeTFs[i].position));
                    Vector2 dayPoint = Vector2Add(activeTFs[i].position, Vector2Scale(delta, SHIPSPEED * DAY_LENGTH));
                    if(dayActive){
                        DrawLineEx(WorldToScreen(activeTFs[i].position), WorldToScreen(target), 1, DARKGRAY);
                    }else{
                        DrawLineEx(WorldToScreen(activeTFs[i].position), WorldToScreen(dayPoint), 2, WHITE);
                        DrawLineEx(WorldToScreen(dayPoint), WorldToScreen(target), 1,  DARKGRAY);
                    }

                    // DrawLine(WorldToScreen(tfs[i].position).x, WorldToScreen(tfs[i].position).y, target.x, target.y, WHITE);
                }
                DrawCircle(pos.x, pos.y, 3, BLUE); 
            }else{

                if(validPath){
                    Vector2 target = activeTFs[i].destination;
                    Vector2 delta = Vector2Normalize(Vector2Subtract( target, activeTFs[i].position));
                    Vector2 dayPoint = Vector2Add(activeTFs[i].position, Vector2Scale(delta, SHIPSPEED * DAY_LENGTH));
                    if(dayActive){
                        DrawLineEx(WorldToScreen(activeTFs[i].position), WorldToScreen(target), 1, DARKGRAY);
                    }else{
                        DrawLineEx(WorldToScreen(activeTFs[i].position), WorldToScreen(dayPoint), 1.5, GRAY);
                        DrawLineEx(WorldToScreen(dayPoint), WorldToScreen(target), 1,  DARKGRAY);
                    }
                }
                DrawCircle(WorldToScreen(activeTFs[i].position).x, WorldToScreen(activeTFs[i].position).y, 3, GRAY); 
            }
            DrawText(activeTFs[i].name, tfpos.x - 15, tfpos.y - 20, 1, WHITE);
        }else{
            if(IsKeyPressed(KEY_S)){
                DrawCircle(WorldToScreen(activeTFs[i].position).x, WorldToScreen(activeTFs[i].position).y, 3, RED); 
                DrawText("OPFOR", tfpos.x - 30, tfpos.y - 20, 1, RED);
            }
        }
    }

}

void MapUIRender(){
    float diff = (WIDTH - HEIGHT) * 0.4;
    int border = 30;
    float dayscaler = 245;
    float sct = worldTime * dayscaler;  
    float td = sct / (60.00 * 24.00);
    int days = floorf(td);
    int hrs = (td - days) * 24;
    const char * str = TextFormat("%ddays, %dhrs", days, hrs);
    DrawText(str, diff + border, border - 10, 12, GRAY);
    DrawText(rightBar.charArray, WIDTH * RSCALE - border - diff - 25, 35, 18, GRAY);

    for(int i =0 ;i < taskForceCount; i++){
        if(activeTFs[i].selected){
            DrawText(activeTFs[i].name, WIDTH * RSCALE - border - diff - 25, 200, 18, WHITE);
            Vector3 col = (Vector3){1, 1, 1};

            DotShaderValues(&generalShader, 0.2, 12, col);
            SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);
            BeginShaderMode(generalShader.shader);
            rlBegin(RL_TRIANGLES);
            // rlColor4ub(255, 255, 255, 255);
            for(int j = 0; j < activeTFs[i].shipCount; j++){
                RenderShipIconAtPoint(activeTFs[i].ships[j], 60, (Vector2){WIDTH * RSCALE * 0.91, 250 + j * 100});
                // DrawText(tfs[i].ships[j]->name, cruiser.polyCenter.x, cruiser.polyCenter.y, 12, WHITE);
            }
            rlEnd();
            rlSetTexture(0); 
            EndShaderMode();

            for(int j = 0; j < activeTFs[i].shipCount; j++){
                DrawText(activeTFs[i].ships[j]->shipName, WIDTH * RSCALE * 0.87 ,280 + j * 100, 15, GRAY);
            }
        }
    }
}
#pragma endregion
