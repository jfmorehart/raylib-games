#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Island island[ISLANDCOUNT];

typedef struct{
    bool team;
    Vector2 wPos;
    float angle;
    float scale;
} Ship;

#define SHIPTURN 2

int shipCount = 5;
Ship ships[5];

void RunOnStart(){
    deltaTime = 1.0 / FRAMERATE;

    srand(time(NULL));
    for(int i = 0; i < ISLANDCOUNT; i++){
        island[i] = CreateIsland(); 
    }

    for(int i = 0; i < shipCount; i++){
        ships[i] = (Ship){true, RVec(0.5), RVec(5).x, Clamp(RVec(0.03).x, 0.01, 0.03)};
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

        // Vector2 screenCenterPixels = (Vector2){WIDTH * 0.501, HEIGHT * 0.501};
        // Vector2 screenCenterWorld = ScreenToWorld((screenCenterPixels));
        // Edge segment = {screenCenterWorld, mousePos};
        // AllIslandsIntersect(island, segment);
    }

    if(IsKeyPressed(KEY_R)){
        for(int i = 0; i < ISLANDCOUNT; i++){
            island[i] = CreateIsland(); 
        }
        for(int i = 0; i < shipCount; i++){
            ships[i] = (Ship){true, RVec(0.5), RVec(5).x, Clamp(RVec(0.03).x, 0.01, 0.03)};
        }   
  
    }
}
Vector2 VfromAngle(float angle) {return (Vector2){cos(angle), sin(angle)};};

float Path2Target(const Ship *ship, int rays, float fanAngle, Vector2 target){

    Vector2 d2m = Vector2Subtract(target, ship->wPos);
    float angleToTarget= atan2(d2m.y, d2m.x);
    float tdist = Vector2Length(d2m);

    float bestAngle = ship->angle + PI;

    //float angle = ship->angle + (i * (fanAngle / (rays-1))) - (fanAngle * 0.5);

    float best_a2T = PI;

    for(int i = 0; i < rays; i++){
        float alternate = 0;
        if(i % 2 == 0){
            alternate = 1;
        }else{
            alternate = -1;
        }

        float angle = ship->angle + (i * (0.5 * fanAngle / (rays-1))) * alternate;
        float a2T = fabsf(sAngle(angleToTarget, angle));

        Vector2 delta = VfromAngle(angle);
        Edge segment = {ship->wPos, Vector2Add(Vector2Scale(delta, tdist), ship->wPos)};
        Hit hit = AllIslandsIntersect(island, segment);

        if(hit.hit){

            float d = Vector2DistanceSqr(ship->wPos, hit.hitPosition);
            if(d >= tdist * tdist && a2T < best_a2T){
                best_a2T = a2T;
                bestAngle = angle;
            }
        }else if(a2T < best_a2T)
        {
            best_a2T = a2T;
            bestAngle = angle;
        }
    }
    return bestAngle;
}

void RenderShip(const Ship *ship){
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 2);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale, sin(ship->angle +PI * 0.5) * ship->scale};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(Vector2Add(ship->wPos, right), Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(Vector2Add(ship->wPos, Vector2Negate(right)), Vector2Scale(forward, -0.5));
    
    DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), WHITE);
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

        float angle = Path2Target(&ships[i], 8, PI * 1, mousePos);
        float diff = sAngle(ships[i].angle, angle);
        if(diff < -0.01){
            ships[i].angle -= deltaTime * SHIPTURN;
        }else if(diff >= 0.01){
            ships[i].angle += deltaTime * SHIPTURN;
        }
        ships[i].wPos = Vector2Add(ships[i].wPos, Vector2Scale(VfromAngle(ships[i].angle), deltaTime * 0.3));
    }
    // RenderShip(&(Ship){true, {0, 0}, appTime, 0.1 * (1 + sin(appTime))});

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
