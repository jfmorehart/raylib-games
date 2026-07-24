#pragma once
#include "globals.h"
#include "helpers.h"
#include <stdio.h>
#include "raymath.h"
#include "rlgl.h"

#include "mapshaders.h"
#include "islands.h"
#include "filesystem.h"

#define HORIZON_Y 0

extern DotShader generalShader;
extern DotShader skyShader;
extern DotShader waterShader;

Font cutfont;

PolyPoly sub;
PolyPoly capt;
PolyPoly conning;

void InitCutScene(){
    printf("init cut scene\n");
    cutfont = LoadFont("assets/jackinput.ttf");
    //setup Ship CONSTANTS (overwrite from mapscene)
    DotShaderValues(&generalShader,0.2, 120, (Vector3){1, 1, 1});
    sub = LoadPolyFile("sub.poly");
    conning = LoadPolyFile("conning.poly");
    capt = LoadPolyFile("capt.poly");
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

void UpdateCutScene(){
    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&skyShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&waterShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    int grey = 10;
    ClearBackground((Color){ grey, grey,grey , 255 });

    Vector3 col = (Vector3){0.2, 0.2, 0.2};
    // DotShaderValues(&skyShader, 0.2, 80, col);
    // BeginShaderMode(skyShader.shader);

    // DrawRectangle(0, 0, WIDTH, HEIGHT / 2, BLUE);
    // EndShaderMode();

    // col = (Vector3){0.1, 0.1, 0.1};
    // DotShaderValues(&waterShader, 0.2, 130, col);
    // BeginShaderMode(waterShader.shader);

    // DrawRectangle(0, HEIGHT/ 2, WIDTH, HEIGHT, BLUE);
    // EndShaderMode();

    // // col = (Vector3){0, 0, 0};
    // // DotShaderValues(&generalShader, 0.2, 50, col);
    // // BeginShaderMode(generalShader.shader);

    // float tfac = unscaledTime * 0.1;
    // // DrawHorizonObj(-1, 10, 5, 10 + tfac);
    // DrawHorizonObj(50, 10, 5, 20 - tfac);
    // DrawHorizonObj(-50, 10, 5, 22 - tfac);
    // DrawHorizonObj(200, 10, 5, 25 - tfac);
    // // DrawHorizonObj(-50, 10, 5, 8 + tfac);
    // // DrawHorizonObj(-40, 15, 5, 3 + tfac);
      

    // col = (Vector3){0.1, 0.1, 0.1};
    // DotShaderValues(&waterShader, 0.2, 130, col);
    // BeginShaderMode(waterShader.shader);

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

void CutSceneUIUpdate(){
    // DrawTextPro(cutfont, "enemy force spotted", Vector2Zero(), Vector2Zero(), 0, 30, 1,WHITE);

     DrawTextPro(cutfont, "we are losing", (Vector2){WIDTH * 0.3, HEIGHT + 30}, Vector2Zero(), 0, 30, 1,WHITE);
}