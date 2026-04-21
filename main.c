#include "battlescene.h"
#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "UI.h"
#include "mapscene.h"

#include <alloca.h>
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum Scene{
    Menu,
    Map,
    Battle
}Scene;
Scene currentScene = Map;

Font font;
Window hello = {BOTTOM_RIGHT, {300, 150}, "Yooo", 32, "whats up", 12};

void RunOnStart(){

    routines[0] = (Routine){"TimeRoutine", false, true, -999, 2, 2, TimeRoutine};
    routines[1] = (Routine){"FocusRoutine", false, true, -999, 1, 1, FocusRoutine};
    routineCount = 2; //update with full number!

    InitWindow(WIDTH, HEIGHT, "raylib");

    font = LoadFont("mecha.ttf");
    // font = LoadFont("resources/sprite_fonts/alpha_beta.png");

    worldScale = 1;
    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0) -30;

    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);
    xBounds = (Vector2){ScreenToWorld((Vector2){0, 0}).x, ScreenToWorld((Vector2){WIDTH, 0}).x}; 
    yBounds = (Vector2){ScreenToWorld((Vector2){0, 0}).y, ScreenToWorld((Vector2){0, HEIGHT}).y}; 

    cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});

    printf("xBounds = %.2f, %.2f\n", xBounds.x, xBounds.y);
    printf("yBounds = %.2f, %.2f\n", yBounds.x, yBounds.y);

    timeScale = 0.1;

    fixedDeltaTime = 1.0 /FRAMERATE;

    srand(time(NULL));

    InitMapScene();
    ShaderInit();
}

void ExecuteRoutines(){
    for(int i = 0; i < routineCount; i++){
        if(routines[i].isActive){
            routines[i].runWhileActive(&routines[i]);
        }
    }
}

int main(void)
{
    RunOnStart();
    while (!WindowShouldClose()) {

        frameCount++;
        frames_fudged += 1.0 * timeScale;
        scaledDeltaTime = (timeScale / (FRAMERATE));
        scaledTime = (float)frames_fudged / FRAMERATE;
        unscaledTime = (float)frameCount / FRAMERATE;

        ExecuteRoutines();

        BeginDrawing();

        if(IsKeyDown(KEY_B)){
            currentScene = Battle;
        }
        if(IsKeyDown(KEY_M)){
            currentScene = Map;
        }
        switch(currentScene){
            case Menu:
            break;
            case Map:
                MapInputLoop();
                MapFrameLoop();
                break;
            case Battle:
                BattleFrameLoop();
        }

        RenderWindow(&hello, &font);
        EndDrawing();
    }

    UnloadShaders();
    CloseWindow();
    return 0;
}
