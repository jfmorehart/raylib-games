#include "globals.h"
#include "helpers.h"
#include <stdio.h>
#include "raymath.h"
#include "rlgl.h"

#include "mapshaders.h"
#include "islands.h"
#include "filesystem.h"
#include "cutscene.h"


extern DotShader generalShader;
extern DotShader skyShader;
extern DotShader waterShader;
extern DotShader cut_foamShader;

extern Vector2 worldZero;

Font cutfont;

PolyPoly sub;
PolyPoly capt;
PolyPoly conning;
float sceneDurations[] = {5, 5, 5, 5, 5};
float startCutsceneTime;

Vector2 focusTarget;

void InitCutScene(){
    worldScale = 2;
    focusTarget = cameraPosition;
    cameraPosition = worldZero;
    printf("init cut scene\n");
    cutfont = LoadFont("assets/jackinput.ttf");
    //setup Ship CONSTANTS (overwrite from mapscene)
    DotShaderValues(&generalShader,0.2, 120, (Vector3){1, 1, 1});
    sub = LoadPolyFile("sub.poly");
    conning = LoadPolyFile("conning.poly");
    capt = LoadPolyFile("capt.poly");

    startCutsceneTime = unscaledTime;
}

void SetCutscene(Cutscene cut)
{
    activeCutscene = cut;
}

void DrawHorizonObj(float azimuth, float worldWidth, float worldHeight, float distance){
    float invd2 = 1 /(distance * distance) ;
    float width = worldWidth * invd2;
    float height = worldHeight * invd2;

    Vector2 v1 = (Vector2){-width * 0.5 + azimuth * invd2, HORIZON_Y};
    Vector2 v2 =(Vector2){width * 0.5 + azimuth * invd2, HORIZON_Y};
    Vector2 v3 = (Vector2){azimuth * invd2, HORIZON_Y + height};

    Vector2 v4 = (Vector2){azimuth * invd2, HORIZON_Y - height * 0.2};
    DrawTriangle(WorldToScreen(v1), WorldToScreen(v2), WorldToScreen(v3), RED);
    DrawTriangle(WorldToScreen(v1), WorldToScreen(v4), WorldToScreen(v2), RED);
}

void RenderPoly(PolyPoly todraw){
    for(int i = 0; i < todraw.islandLength; i++){
        // todraw.islands[i].scale = 1;
        // todraw.islands[i].relativePosition = Vector2Zero();
        Render(&todraw.islands[i]);
        // DrawTriangle(WorldToScreen((Vector2){100, 0}),WorldToScreen((Vector2){1, 0}), WorldToScreen((Vector2){0, 0}), WHITE);
        // DrawTriangle(WorldToScreen((Vector2){0, 0}),WorldToScreen((Vector2){100, 0}), WorldToScreen((Vector2){0, 1}), WHITE);
    }
}


void DrawShipsOnHorizon(){
    Vector3 col = (Vector3){0.2, 0.2, 0.2};
    DotShaderValues(&skyShader, 0.2, 80, col);
    BeginShaderMode(skyShader.shader);

    DrawRectangle(0, 0, WIDTH, HEIGHT / 2, BLUE);
    EndShaderMode();

    col = (Vector3){0.1, 0.1, 0.1};
    DotShaderValues(&waterShader, 0.2, 130, col);
    BeginShaderMode(waterShader.shader);

    DrawRectangle(0, HEIGHT/ 2, WIDTH, HEIGHT, BLUE);
    EndShaderMode();

    // col = (Vector3){0, 0, 0};
    // DotShaderValues(&generalShader, 0.2, 50, col);
    // BeginShaderMode(generalShader.shader);

    float tfac = (unscaledTime - startCutsceneTime) * 0.1;
    // DrawHorizonObj(-1, 10, 5, 10 + tfac);
    DrawHorizonObj(50, 10, 5, 20 - tfac);
    DrawHorizonObj(-50, 10, 5, 22 - tfac);
    DrawHorizonObj(200, 10, 5, 25 - tfac);
    // DrawHorizonObj(-50, 10, 5, 8 + tfac);
    // DrawHorizonObj(-40, 15, 5, 3 + tfac);
}

void DrawSinking(){
    Vector3 col = (Vector3){0.2, 0.2, 0.2};
    DotShaderValues(&skyShader, 0.2, 80, col);
    BeginShaderMode(skyShader.shader);

    DrawRectangle(0, 0, WIDTH, HEIGHT / 2, BLUE);
    EndShaderMode();

    col = (Vector3){0.1, 0.1, 0.1};
    DotShaderValues(&waterShader, 0.2, 130, col);
    BeginShaderMode(waterShader.shader);

    DrawRectangle(0, HEIGHT/ 2, WIDTH, HEIGHT, BLUE);
    EndShaderMode();

    int w = 60;
    int h = 30;
    int rscale = 8;
    float tfac = roundf(unscaledTime * rscale) / rscale - startCutsceneTime;
    tfac *= 10;
    h += tfac - tfac * tfac * 0.04;
    w -= tfac / 2 + tfac * tfac * 0.01;
     w = fmax(w, h / 2);
    Vector2 center = (Vector2){WIDTH / 2 - 30, HEIGHT / 2 + 10};
    Vector2 lc = (Vector2){ center.x - w, center.y};
    Vector2 rc = (Vector2){ w + center.x, center.y};
    Vector2 top = (Vector2){center.x - w * 1.3 + h / 10, center.y - h};

    if(h > 0){
        if(activeCutscene == SinkingEnemy){
            DrawTriangle(rc, top, lc, RED);
        }else{
            DrawTriangle(rc, top, lc, WHITE);
        }

    }



    PrepOceanPass(worldZero, 0 , 0);
    BeginShaderMode(cut_foamShader.shader);
    w = fmax(w, 5);
    h = fmax(h, 5);
    DrawRectangle(center.x - w - 10, center.y - 2, 2 * w + 20, 6, GRAY);
    EndShaderMode();
}


void DrawCaptain(){

    Vector3 col = (Vector3){0.1, 0.1, 0.1};
    DotShaderValues(&waterShader, 0.2, 130, col);
    BeginShaderMode(waterShader.shader);

    DrawRectangle(0, HEIGHT, WIDTH, HEIGHT, WHITE);
    // EndShaderMode();

    //sub
    col = (Vector3){0.5, 0.5, 0.5};
    DotShaderValues(&generalShader, 0.25, 250, col);
    BeginShaderMode(generalShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                     
    rlBegin(RL_TRIANGLES);

    RenderPoly(capt);

    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();


    //conning
    col = (Vector3){0.7, 0.7, 0.7};
    DotShaderValues(&generalShader, 0.2, 250, col);
    BeginShaderMode(generalShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                     
    rlBegin(RL_TRIANGLES);

    RenderPoly(conning);

    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();


    //capt
    col = (Vector3){1, 1, 1};
    DotShaderValues(&generalShader, 0.15, 250, col);
    BeginShaderMode(generalShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                     
    rlBegin(RL_TRIANGLES);

    RenderPoly(sub);

    rlEnd();                                                        
    rlSetTexture(0); 
    EndShaderMode();
}


void UpdateCutScene(){

    if(unscaledTime - startCutsceneTime > sceneDurations[(int)activeCutscene]){

        switch(activeCutscene){
            case ShipsOnHorizon:
                cameraPosition = focusTarget;
                worldScale = 0.4;
                printf("set cpos to %f, %f\n", cameraPosition.x, cameraPosition.y);
                SwitchScenes(Battle);
                break;
            case SinkingEnemy:
                SwitchScenes(MapScene);
                break;
            case SinkingFriendly:
                SwitchScenes(MapScene);
                break;
            default:
                break;
        }
        return;
    }

    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&skyShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&waterShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    int grey = 10;
    ClearBackground((Color){ grey, grey,grey , 255 });
    
    switch(activeCutscene){
        case LoneCaptain:
            DrawCaptain();
            break;
        case ShipsOnHorizon:
            DrawShipsOnHorizon();
            break;
        case SinkingEnemy:
            DrawSinking();
            break;
        case SinkingFriendly:
            DrawSinking();
            break;
    }
}

void CutSceneUIUpdate(){
    // DrawTextPro(cutfont, "enemy force spotted", Vector2Zero(), Vector2Zero(), 0, 30, 1,WHITE);

    switch(activeCutscene){
        case LoneCaptain:
            DrawTextPro(cutfont, "once more to sea...", (Vector2){WIDTH * 0.3, HEIGHT + 30}, Vector2Zero(), 0, 30, 1,WHITE);
            break;
        case ShipsOnHorizon:
            DrawTextPro(cutfont, "large enemy force to our southwest...", (Vector2){WIDTH * 0.3, HEIGHT + 30}, Vector2Zero(), 0, 30, 1, WHITE);
            break;
     case SinkingEnemy:
            DrawTextPro(cutfont, "enemy sinking", (Vector2){WIDTH * 0.3, HEIGHT + 30}, Vector2Zero(), 0, 30, 1, WHITE);
            break;
    }
    
}