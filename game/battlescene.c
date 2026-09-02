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
#include "bullets.h"
#include "battlescene.h"

#include "rlgl.h"


#include <math.h>       
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

extern Map mapFromDisk;

extern Vector2 worldZero;
extern PolyPoly cruiser;

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

int fcham;
#define MAX_FIRESTACKS 50
FireStack firestacks[MAX_FIRESTACKS];
float firetick_last;
float firetick_delay = 0.2;


int bulletCham;
int bulletCount = 100;
Bullet bulletPool[100];

int smokeCham;
int smokeCount = 100;
Smoke smokePool[100];

int splashCham;
int splashCount = 100;
Smoke splashPool[100];


void UpdateAndRenderFireStacks(){
    bool tickDamage = unscaledTime - firetick_last > firetick_delay;
    if(tickDamage) firetick_last = unscaledTime;

    rlSetTexture(rlGetTextureIdDefault());          
    rlBegin(RL_QUADS);    

    for(int i = 0; i < MAX_FIRESTACKS; i++){

        if(firestacks[i].alive){
            if(firestacks[i].attached){
                if(firestacks[i].attached->alive){
                    //UPDATE
                    if(tickDamage){
                        firestacks[i].attached->health -= firestacks[i].amount * 0.06;
                        if(firestacks[i].attached->health < 1) 
                        {
                            FireSmoke(firestacks[i].attached->wPos, WorldToPixels(SHIP_EXPLOSION_RADIUS * (R01() * 0.4 + 0.4)));
                            firestacks[i].attached->alive = false;
                            firestacks[i].alive = false;
                        }

                        firestacks[i].amount *= 0.94;
                        if(firestacks[i].amount < 3) firestacks[i].alive = false;
                    }
      
                    //RENDER
                    float angle = firestacks[i].attached->angle;
                    Vector2 objspaceOff = (Vector2){cosf(angle) * firestacks[i].localOffset.x, sinf(angle) * firestacks[i].localOffset.y};
                    // DrawCircleV(WorldToScreen(Vector2Add(objspaceOff, firestacks[i].attached->wPos)), firestacks[i].amount * 0.15, ORANGE);

                    Vector2 c = WorldToScreen(Vector2Add(objspaceOff, firestacks[i].attached->wPos));
                    float r = firestacks[i].amount * 0.15;             
                    rlTexCoord2f(0, 0);                                                                                                
                    rlVertex2f(c.x - r, c.y - r);                                                                                        
                                                                                
                    // bottom-left                                                                                                     
                    rlTexCoord2f(0, 1);                         
                    rlVertex2f(c.x - r, c.y + r);                                 
                                                                                                                                        
                    // bottom-right
                    rlTexCoord2f(1, 1);                                                                                                
                    rlVertex2f(c.x + r, c.y + r);                 
                                                                                
                    // top-right                                                                                                       
                    rlTexCoord2f(1, 0);
                    rlVertex2f(c.x + r, c.y - r);  

                    printf("rendering live firestack %d, \n", i);


                }else{
                    firestacks[i].alive = false;
                    continue;
                }
            }else{
                firestacks[i].alive = false;
                continue;
            }


        }
    }
    rlEnd();                                                        
    rlSetTexture(0); 
}
void ApplyFireStacks(Ship * toship, int amount){
    firestacks[fcham].alive = true;
    firestacks[fcham].amount = amount;
    firestacks[fcham].attached = toship;
    firestacks[fcham].localOffset = RVec(toship->scale);
    printf("applying firestack\n");
}


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

    if(battleOver && unscaledTime - battleOverTime > 3){
        battleOver = false;
        battleOverTime = 0;
        if(wonBattle){
            WonBattleSwitch();
        }else{
            LostBattleSwitch();
        }
    }

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
            mapFromDisk.enemies[i].illuminationThisFrame += scaledDeltaTime * 0.1;
        }
    }
    for(int i = 0; i < MAX_FIRESTACKS; i++){
        if(!firestacks[i].alive)continue;
        if(!firestacks[i].attached) continue;
        if(!firestacks[i].attached->alive) continue;
        
        float angle = firestacks[i].attached->angle;
        Vector2 objspaceOff = (Vector2){cosf(angle) * firestacks[i].localOffset.x, sinf(angle) * firestacks[i].localOffset.y};

        // DrawCircularBeam(Vector2Add(objspaceOff, firestacks[i].attached->wPos), 30, 1, &mapFromDisk, 0.3, scaledDeltaTime);
        DrawCircleV(WorldToScreen(Vector2Add(objspaceOff, firestacks[i].attached->wPos)), firestacks[i].amount * 0.6, WHITE);
        firestacks[i].attached->illuminationThisFrame += firestacks[i].amount * 1 * scaledDeltaTime;
        //DrawCircularBeam(Vector2 start, int beamSegments, float radius, Map *m, float scaleMult, float illumbrightness) {
    }
    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();
    EndBlendMode();

    //CALCULATE ENEMY VISIBILITY
    PrepShipRangePass();
    for(int i = 0; i < mapFromDisk.fcount; i++){
        if(!mapFromDisk.friendlies[i].alive || !mapFromDisk.friendlies[i].includedInScene)continue;
        DrawCircleV(WorldToScreen(mapFromDisk.friendlies[i].wPos), WorldToPixels(BATTLE_SEARCHRANGE * 0.5), WHITE);

        for(int d = 0; d < mapFromDisk.ecount; d++){
            if(!mapFromDisk.enemies[d].alive || !mapFromDisk.enemies[d].includedInScene)continue;
            if(Vector2Distance(mapFromDisk.friendlies[i].wPos, mapFromDisk.enemies[d].wPos) < BATTLE_SEARCHRANGE){
                mapFromDisk.enemies[d].isVisible = true;
                mapFromDisk.friendlies[i].isVisible = true;
            }
            else if (Vector2Distance(mapFromDisk.friendlies[i].wPos, mapFromDisk.enemies[d].wPos) < BATTLE_SEARCHRANGE * 1.5){
                // currentMap.enemies[d].isVisible = true;
                mapFromDisk.enemies[d].illuminationThisFrame += 0.1 * scaledDeltaTime;
            }
        }
    }
    EndShaderMode();


    //STEER SHIPS + FIGHT
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

    //Splashes
    col = (Vector3){0.2, 0.2, 0.2};
    DotShaderValues(&generalShader,0.3, 180, col);
    BeginShaderMode(generalShader.shader);
    UpdateAndRenderBlobs(splashPool, splashCount);
    EndShaderMode();

    EndBlendMode();
    //Explosions!
    col = (Vector3){1, 0.83, 0.25};
    DotShaderValues(&explosionShader, 0.45, 230, col);
    // SetShaderValue(explosionShader.shader, explosionShader.dloc, &col, SHADER_UNIFORM_VEC3);
    BeginShaderMode(explosionShader.shader);
    UpdateAndRenderBlobs(smokePool, smokeCount);
    UpdateAndRenderFireStacks();
    EndShaderMode();
    EndShaderMode();
    


    if(active_fcount < 1 && !battleOver){
        //all friendlies lost
        wonBattle = false;
        battleOver = true;
        battleOverTime = unscaledTime;
        LostBattleSwitch();
    }

    if(liveEnemies < 1 && !battleOver){
        // char fullstring[30] = "livemaps/";
        // strcat(fullstring, mapFromDisk.filename);
        // FILE *fptr = fopen(fullstring, "wb");
        // if(fptr){
        //     // Write some text to the file
        //     fwrite(&mapFromDisk, sizeof(Map), 1, fptr);   
        //     // Close the file
        //     fclose(fptr); 
        //     printf("saved map to livemaps folder");
        // }
        // fread(&mapFromDisk, sizeof(Map),1);
        
        wonBattle = true;
        battleOver = true;
        battleOverTime = unscaledTime;
        // WonBattleSwitch();
    }



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

    int numSel = 0;
    for(int i = 0 ; i < mapFromDisk.fcount; i++){
        if(mapFromDisk.friendlies[i].selected){
            DrawText("Kobayashi Maru", WIDTH * RSCALE - border - diff - 25, 200 + numSel * 80, 18, WHITE);
            Vector3 col = (Vector3){1, 1, 1};

            DotShaderValues(&generalShader, 0.2, 150, col);
            SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);
            BeginShaderMode(generalShader.shader);
            rlBegin(RL_TRIANGLES);
            rlColor4ub(255, 255, 255, 255);
            cruiser.polyCenter = (Vector2){WIDTH * RSCALE * 0.93, 245 + numSel * 80};
            numSel++;
            cruiser.polyScale = 60;
            RenderPolyAsUI(cruiser);
            rlEnd();
            rlSetTexture(0); 
            EndShaderMode();
        }
    }
}