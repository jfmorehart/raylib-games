#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Island island[ISLANDCOUNT];


int shipCount = 5;
Ship ships[5];

void RunOnStart(){
    deltaTime = 1.0 / FRAMERATE;

    srand(time(NULL));
    for(int i = 0; i < ISLANDCOUNT; i++){
        island[i] = CreateIsland(); 
    }

    for(int i = 0; i < shipCount; i++){
        ships[i] = (Ship){true, RVec(0.5), RVec(5).x, 0.01};
    }   
  
    InitWindow(WIDTH, HEIGHT, "raylib");

    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0) -30;

    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);
    xBounds = (Vector2){ScreenToWorld((Vector2){0, 0}).x, ScreenToWorld((Vector2){WIDTH, 0}).x}; 

    ShaderInit();
}

void InputLoop(Vector2 mposSc){

    if(IsMouseButtonDown(0)){
        DrawCircleV(GetMousePosition(), 5, GREEN);
    }

    if(IsKeyPressed(KEY_R)){
        for(int i = 0; i < ISLANDCOUNT; i++){
            island[i] = CreateIsland(); 
        }
        for(int i = 0; i < shipCount; i++){
            ships[i] = (Ship){true, RVec(0.5), RVec(5).x, 0.01};
        }   
  
    }
}

void FrameLoop(){

    ClearBackground((Color){ 24, 24, 80, 255 });

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);
    mousePos_ScreenCoords.y = HEIGHT - mousePos_ScreenCoords.y;
    mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 2); 

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_ScreenCoords);

    for(int i = 0; i < shipCount; i++){
        RenderShip(&ships[i]);
        SteerShip(&ships[i]);
    }

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < ISLANDCOUNT; i++){
        Render(&island[i]);
    }
    EndShaderMode();

    InputLoop(mousePos_ScreenCoords);
}

int main(void)
{
    RunOnStart();
    while (!WindowShouldClose()) {

        frameCount++;
        appTime = (float)frameCount / FRAMERATE;

        BeginDrawing();
        FrameLoop();
        EndDrawing();
    }

    UnloadShaders();
    CloseWindow();
    return 0;
}
