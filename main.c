#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"

#include <alloca.h>
#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern Island island[ISLANDCOUNT];

int shipCount = 5;
Ship ships[5];

int detectablesCount = 10;
Vector2 detectables[10];
bool isdetected[10];


bool timeRoutineActive;
float lastTimeRoutineActivated = -999;
float timeRoutineDelay = 2;
float timeRoutineDuration = 2;

typedef struct Routine{
    const char* name;
    bool isActive;
    bool useUnscaledTime;
    float startTime;
    float duration;
    float delay;
    void (*runWhileActive)(struct Routine *routine);
} Routine;

int routineCount = 5;
Routine routines[5];

Routine *FindRoutineFromName(const char* routineName){
    for(int i = 0; i < routineCount; i++){
        if(routines[i].name == routineName){
            printf("found %s\n", routineName);
            return &routines[i];
        }
    }
}

bool RunRoutine(const char* routineName){
    Routine *routine = FindRoutineFromName(routineName);
    //too early
    //todo fix time
    if(unscaledTime - routine->startTime < routine->delay) return false;
    routine->startTime = unscaledTime;
    routine->isActive = true;
    return true;
}

void TimeRoutine(Routine *routine){

    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    float dt = 1 - fabs(0.5 - pct) / 0.5;
    timeScale = 2 * dt + 0.5;

    if(pct > 1){
        routine->isActive= false;
        timeScale = 0.1;
        printf("End Time Routine");
    }
}

Vector2 startingCameraPos;
float startingZoom;
float endZoom;
Vector2 focusTarget;
bool isZoomed;
void FocusRoutine(Routine *routine){
    float runtime = (unscaledTime - routine->startTime);
    float pct = runtime / routine->duration;
    pct = pow(pct, 0.5);
    cameraPosition = Vector2Lerp(startingCameraPos, focusTarget, pct);
    worldScale = Lerp(startingZoom, endZoom, pct);

    if(pct > 1){
        isZoomed = !isZoomed;
        routine->isActive= false;
        printf("End Time Routine");
    }
}

void RunOnStart(){

    routines[0] = (Routine){"TimeRoutine", false, true, -999, 2, 2, (void *)TimeRoutine};
    routines[1] = (Routine){"FocusRoutine", false, true, -999, 1, 1, (void *)FocusRoutine};

    InitWindow(WIDTH, HEIGHT, "raylib");

    worldScale = 1;
    WIDTH = GetMonitorHeight(0) -30; //GetMonitorWidth(0);
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
    for(int i = 0; i < ISLANDCOUNT; i++){
        island[i] = CreateIsland(); 
    }

    for(int i = 0; i < shipCount; i++){
        ships[i] = (Ship){true, RandomWorldPoint(), R01() * 5, 0.01};
    }   

    for(int i = 0; i < detectablesCount; i++){
        detectables[i] = RandomWorldPoint();
    } 

    ShaderInit();
}

void ExecuteRoutines(){
    for(int i = 0; i < routineCount; i++){
        if(routines[i].isActive){
            routines[i].runWhileActive(&routines[i]);
        }
    }
}

void InputLoop(){

    if(IsMouseButtonDown(0)){
        if(IsPointWithinIslands(ScreenToWorld(GetMousePosition()))){
            DrawCircleV(GetMousePosition(), 5, RED);
        }else{
            DrawCircleV(GetMousePosition(), 5, GREEN);
        }
  
    }
    if(IsKeyPressed(KEY_SPACE)){
        bool run = RunRoutine("TimeRoutine");
        printf("pressed space %u\n", run);
    }
     if(IsKeyPressed(KEY_F)){

        bool run = RunRoutine("FocusRoutine");

        if(run){
            startingCameraPos = cameraPosition;
            startingZoom = worldScale;
            focusTarget = ScreenToWorld(GetMousePosition());
            if(isZoomed){
                endZoom = 1;
            }else{
                endZoom = 0.1;
            }


        }

        printf("pressed F %u\n", run);
    }

    // if(IsKeyDown(KEY_Z)){
    //     worldScale = 0.2;
    // }else{
    //     worldScale = 1;
    // }
    if(IsKeyDown(KEY_D)){
        cameraPosition.x += fixedDeltaTime;
    }
    if(IsKeyDown(KEY_A)){
        cameraPosition.x -= fixedDeltaTime;
    }
     if(IsKeyDown(KEY_W)){
        cameraPosition.y += fixedDeltaTime;
    }
    if(IsKeyDown(KEY_S)){
        cameraPosition.y -= fixedDeltaTime;
    }

    if(IsKeyPressed(KEY_R)){
        for(int i = 0; i < ISLANDCOUNT; i++){
            island[i] = CreateIsland(); 
        }
        for(int i = 0; i < shipCount; i++){
            ships[i] = (Ship){true,  RandomWorldPointNoIsland(), RVec(5).x, 0.01};
        }   
        for(int i = 0; i < detectablesCount; i++){
            detectables[i] = RandomWorldPointNoIsland();
        } 
    }
}

void FrameLoop(){

    ClearBackground((Color){ 1, 1, 1, 255 });

    float gridSize = 0.2;
    for(float x = xBounds.x; x < xBounds.y; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){x, -3}), WorldToScreen((Vector2){x, 3}), GRAY);
    }
    for(float x = yBounds.y; x < yBounds.x; x+= gridSize){
        DrawLineV(WorldToScreen((Vector2){-3, x}), WorldToScreen((Vector2){3, x}), GRAY);
    }

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);
    mousePos_ScreenCoords.y = HEIGHT - mousePos_ScreenCoords.y;
    mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 2); 

    //Set shader variables and draw ocean
    PrepOceanPass(mousePos_ScreenCoords);
    EndOceanPass(); //flush buffer

    for(int d = 0; d < detectablesCount; d++){
        isdetected[d] = false;
    }

    PrepShipRangePass();
    for(int i = 0; i < shipCount; i++){
        DrawCircleV(WorldToScreen(ships[i].wPos), WorldToPixels(SHIP_SEARCHRANGE), WHITE);

        for(int d = 0; d < detectablesCount; d++){
            if(Vector2Distance(ships[i].wPos, detectables[d]) < SHIP_SEARCHRANGE){
                isdetected[d] = true;
            }
        }
    }
    EndOceanPass();


    for(int d = 0; d < detectablesCount; d++){
        if(isdetected[d]){
            DrawCircleV(WorldToScreen(detectables[d]), 8, RED);
        }
    }
   
    for(int i = 0; i < shipCount; i++){
        RenderShip(&ships[i]);
        SteerShip(&ships[i]);
    }

    BeginShaderMode(islandShader_frag);
    for(int i = 0; i < ISLANDCOUNT; i++){
        Render(&island[i]);
    }
    EndShaderMode();
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
        FrameLoop();
        InputLoop();
        EndDrawing();
    }

    UnloadShaders();
    CloseWindow();
    return 0;
}
