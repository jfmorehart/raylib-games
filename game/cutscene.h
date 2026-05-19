#pragma once
#include "globals.h"
#include "helpers.h"
#include <stdio.h>
#include "raymath.h"

#include "mapshaders.h"

#define HORIZON_Y 0

extern DotShader generalShader;
extern DotShader skyShader;
extern DotShader waterShader;

Font cutfont;

void InitCutScene(){
    printf("init cut scene\n");
    cutfont = LoadFont("assets/jackinput.ttf");
    //setup Ship CONSTANTS (overwrite from mapscene)
    DotShaderValues(&generalShader,0.2, 120, (Vector3){1, 1, 1});
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

void UpdateCutScene(){
    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&skyShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    FrameRefreshShader(&waterShader, unscaledTime, cameraPosition, worldScale, Vector2Zero());
    int grey = 10;
    ClearBackground((Color){ grey, grey,grey , 255 });

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

    float tfac = unscaledTime * 0.1;
    DrawHorizonObj(-1, 10, 5, 10 + tfac);
    DrawHorizonObj(50, 10, 5, 20 + tfac);
    DrawHorizonObj(-50, 10, 5, 8 + tfac);
    DrawHorizonObj(-40, 15, 5, 3 + tfac);

    // EndShaderMode();
}

void CutSceneUIUpdate(){
    DrawTextPro(cutfont, "enemy force spotted", Vector2Zero(), Vector2Zero(), 0, 30, 1,WHITE);
}