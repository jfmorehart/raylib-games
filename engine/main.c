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
#include "routines.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Font font;
Window hello = {BOTTOM_RIGHT, {300, 150}, "Yooo", 32, "whats up", 12};
Vector2 worldZero;

void RunOnStart(){


    scenes[0] = (Scene){Menu, NULL};
    scenes[1] = (Scene){Map, InitMapScene};
    scenes[2] = (Scene){Battle, InitBattleScene};

    currentScene = Map;

    routines[0] = (Routine){"TimeRoutine", false, true, -999, 2, 2, TimeRoutine};
    routines[1] = (Routine){"FocusRoutine", false, true, -999, 1, 1, FocusRoutine};
    routines[2] = (Routine){"SwitchToBattleRoutine", false, true, -999, 1, 1, SwitchToBattleRoutine};
    routineCount = 3; //update with full number!


    InitWindow(WIDTH, HEIGHT, "raylib");
    
    ChangeDirectory(GetApplicationDirectory());
    font = LoadFont("mecha.ttf");
    // font = LoadFont("resources/sprite_fonts/alpha_beta.png");

    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0);

    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);
    
    ToggleBorderlessWindowed();

    worldScale = 2;
    cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});    
    worldZero = cameraPosition;

    float diff = (WIDTH - HEIGHT) * 0.3;
    int border = 30;

    xBounds = (Vector2){ScreenToWorld((Vector2){diff, 0}).x, ScreenToWorld((Vector2){WIDTH - diff, 0}).x}; 
    yBounds = (Vector2){ScreenToWorld((Vector2){border, 0}).y, ScreenToWorld((Vector2){0, HEIGHT - border}).y}; 

    cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});

    printf("xBounds = %.2f, %.2f\n", xBounds.x, xBounds.y);
    printf("yBounds = %.2f, %.2f\n", yBounds.x, yBounds.y);

    timeScale = 0.1;

    fixedDeltaTime = 1.0 /FRAMERATE;

    srand(time(NULL));

    RandomizeMap();
    InitMapScene();
    ShaderInit();
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
            InitBattleScene();
            currentScene = Battle;
        }
        if(IsKeyDown(KEY_M)){
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

        // RenderWindow(&hello, &font);

        float diff = (WIDTH - HEIGHT) * 0.4;
        int border = 30;
        DrawRectangle(0, 0, diff, HEIGHT, BLACK);//, int posY, int width, int height, Color color)
        DrawRectangle(WIDTH-  diff, 0, diff, HEIGHT, BLACK);
        DrawRectangle(diff, 0, WIDTH - diff * 2,border, BLACK);
        DrawRectangle(diff, HEIGHT - border, WIDTH - diff * 2, border, BLACK);
        EndDrawing();
    }

    UnloadShaders();
    CloseWindow();
    return 0;
}
