#include "battlescene.h"
#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "UI.h"
#include "routines.h"
#include "editorscene.h"
#include "mapscene.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Font font;
Window hello = {BOTTOM_RIGHT, {300, 150}, "Yooo", 32, "whats up", 12};
Vector2 worldZero;
RenderTexture2D targetTex;

#define RSCALE 2

void RunOnStart(){

    scenes[0] = (Scene){Menu, NULL};
    scenes[1] = (Scene){MapScene, InitMapScene};
    scenes[2] = (Scene){Battle, InitBattleScene};
    scenes[3] = (Scene){Editor, InitEditorScene};

    currentScene = MapScene;

    routines[0] = (Routine){"TimeRoutine", false, true, -999, 2, 2, TimeRoutine};
    routines[1] = (Routine){"FocusRoutine", false, true, -999, 1, 1, FocusRoutine};
    routines[2] = (Routine){"SwitchToBattleRoutine", false, true, -999, 1, 1, SwitchToBattleRoutine};
    routineCount = 3; //update with full number!

    InitWindow(WIDTH, HEIGHT, "raylib");
    
    ChangeDirectory(GetApplicationDirectory());
    font = LoadFont("mecha.ttf");

    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0);

    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);


    WIDTH = WIDTH / RSCALE;
    HEIGHT = HEIGHT / RSCALE;
    targetTex = LoadRenderTexture(WIDTH, HEIGHT);
    SetTextureFilter(targetTex.texture, TEXTURE_FILTER_POINT);
    
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

        BeginTextureMode(targetTex);

        scaledDeltaTime = GetFrameTime() * timeScale;
        scaledTime += scaledDeltaTime;

        unscaledTime += GetFrameTime();


        mousePos_ScreenCoords = GetMousePosition();
        mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 1.00 / RSCALE);
        mousePos = ScreenToWorld(mousePos_ScreenCoords);

        mousePos_fragCoords.y = HEIGHT * RSCALE - mousePos_ScreenCoords.y;
        mousePos_fragCoords = Vector2Scale(mousePos_fragCoords, 2 * RSCALE); 

        ExecuteRoutines();

        BeginDrawing();

        if(IsKeyPressed(KEY_P)){
            SwitchScenes(Editor);
        }

        switch(currentScene){
            case Menu:
            break;
            case MapScene:
                MapInputLoop();
                MapFrameLoop();
                break;
            case Battle:
                BattleFrameLoop();
            break;
            case Editor:
               EditorFrameLoop();
            break;
        }

        // RenderWindow(&hello, &font);

        float diff = (WIDTH - HEIGHT) * 0.4;
        int border = 3;
        DrawRectangle(0, 0, diff, HEIGHT, BLACK);//, int posY, int width, int height, Color color)
        DrawRectangle(WIDTH-  diff, 0, diff, HEIGHT, BLACK);
        DrawRectangle(diff, 0, WIDTH - diff * 2,border, BLACK);
        DrawRectangle(diff, HEIGHT - border, WIDTH - diff * 2, border, BLACK);

        EndTextureMode();
        int ws = WIDTH * RSCALE; 
        int hs = HEIGHT * RSCALE;
        Rectangle dest = (Rectangle){0, 0, ws, hs};
        DrawTexturePro(targetTex.texture, (Rectangle){0, 0, WIDTH, -HEIGHT}, dest, Vector2Zero(), 0, WHITE);
        EndDrawing();

    }

    UnloadShaders();
    CloseWindow();
    return 0;
}
