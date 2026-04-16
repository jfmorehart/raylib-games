#include "raylib.h"
#include "raymath.h"
#include <math.h>       

#define FRAMERATE 120
int WIDTH;
int HEIGHT;
Vector2 screenVec;
float deltaTime = 1.0 / FRAMERATE;

float PixelsToWorld(float pixels) {                     
    return 1.5 *  pixels / HEIGHT;                                      
}                                                                                                                        
   
float WorldToPixels(float world) {                                                                                       
    return world * HEIGHT * 0.66;                              
}          

Vector2 WorldToScreen(Vector2 worldPos){
    return (Vector2){(0.5 * worldPos.x + 0.5) * HEIGHT, (0.5 * -worldPos.y + 0.5) * HEIGHT};
}
Vector2 ScreenToWorld(Vector2 screenPos){
    screenPos = Vector2Scale(Vector2SubtractValue(Vector2Scale(screenPos, (float)1 / HEIGHT), 0.5), 2);
    screenPos.y *= -1;
    return screenPos;
}

Vector2 xBounds;
Vector2 position = {0, 0};
Vector2 velocity = {0.5, 1};

float lastRicochetFrame;
int frameCount;
bool colliding;
float gravityScale = 1;

float elasticity = 0.999;
Vector2 weightedMousePos;
Vector2 currentMousePos;
float mouseVel;

typedef struct {

    bool active;
    int collisions;
    Vector2 position;
    Vector2 velocity;
    int pixelRadius;
    float radius;
} Ball;

Ball balls[200];  
int ballCount = 200;
int cham;

int totalCollisions_lastframe;
int totalCollisions;
float totalSpeed;
float totalSpeed_lastFrame;

void RenderBall(Ball *b){
    Vector2 position = b->position;
    Vector2 velocity = b->velocity;

    if(position.x + b->radius > xBounds.y ) {
        velocity.x *= -elasticity;
        position.x = xBounds.y - b->radius;
        b->collisions++;    
    }
    if(position.x - b->radius < xBounds.x){
        position.x = xBounds.x + b->radius;
        velocity.x *= -elasticity;
        b->collisions++;
    }
    if(position.y + b->radius > 1){
        position.y = 1 - b->radius;
        velocity.y *= -(elasticity + 0.02);
        b->collisions++;
    }
    if( position.y - b->radius < -1){
        position.y = -1 + b->radius;
        velocity.y *= -elasticity;
        velocity.y += 0.01;
        b->collisions++;
    }

    if(Vector2Distance(position, currentMousePos) < b->radius && !colliding){
        float len = Vector2Length(velocity) + mouseVel * 5;
        velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(position, currentMousePos)), len * elasticity);
        b->collisions++;    
        colliding = true;
    }else{
        colliding = false;
    }
    velocity.y -= gravityScale * deltaTime;
    b->position = position;
    b->velocity = velocity;
    b->position = Vector2Add(b->position, Vector2Scale(b->velocity, deltaTime));
    totalCollisions += b->collisions;
    float myspeed = Vector2Length(b->velocity);
    totalSpeed += myspeed;
    float pctSpeed = myspeed / totalSpeed_lastFrame;
    float pctColl = (float)b->collisions / totalCollisions_lastframe;
    float dS = pctSpeed / (1.00 / cham);
    float dC = pctColl / (1.00 /cham);
    DrawCircleV(WorldToScreen(position), b->pixelRadius, (Color){0, 0,fminf(powf(dS, 2) * 128, 255), 255});    
}

void Render (){ 

    frameCount++;
    Vector2 mpos = GetMousePosition();
    currentMousePos = ScreenToWorld(mpos);

    float weight = 1 - deltaTime * 2;
    weightedMousePos = Vector2Add(Vector2Scale(weightedMousePos, weight), Vector2Scale(currentMousePos, 1 - weight));
    mouseVel = Vector2Distance(currentMousePos, weightedMousePos);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && cham < ballCount - 1)
    {
        balls[cham] = (Ball){true, 0, (Vector2){0.5, 0.5}, (Vector2){0, 0}, 20, PixelsToWorld(20)};
        cham++;
    }
    for(int i = 0; i < ballCount; i++){
     
        if(balls[i].active)
        {
            RenderBall(&balls[i]);
            for(int j = i; j < ballCount; j++){
                if(i == j) continue;
                if(balls[j].active)
                {
                    float d = Vector2Distance(balls[i].position, balls[j].position);
                    if(balls[i].radius + balls[j].radius > d){
                        //collide!
                        float overlap = balls[i].radius + balls[j].radius - d;
                        float len = 0.5 * (Vector2Length(balls[i].velocity) + Vector2Length(balls[j].velocity));
                        Vector2 delta = Vector2Subtract(balls[j].position, balls[i].position); //pointing at J
                        Vector2 deltaNormal =Vector2Normalize(delta);
                        balls[j].velocity = Vector2Scale(deltaNormal, len * elasticity); 
                        balls[j].position = Vector2Add(balls[j].position, Vector2Scale(deltaNormal, overlap * 0.6));
                        balls[i].collisions++;
                        balls[j].collisions++;
                        balls[i].velocity = Vector2Scale(Vector2Negate(deltaNormal), len * elasticity); 
                        balls[i].position = Vector2Add(balls[i].position, Vector2Scale(Vector2Negate(deltaNormal), (overlap * 0.6)));
                    }
                }
            }
        }
    }
    totalCollisions_lastframe = totalCollisions;
    totalSpeed_lastFrame = totalSpeed;
    totalCollisions = 0;
    totalSpeed = 0;

    DrawText(TextFormat("mpos(%0.2f, %0.2f)", mpos.x, mpos.y), 50, 70, 20, WHITE);        
    DrawText(TextFormat("spos (%0.2f, %0.2f)", currentMousePos.x, currentMousePos.y), 50, 50, 20, WHITE);        
    DrawText(TextFormat("dimensions (%0.2f, %0.2f)", screenVec.x, screenVec.y), 50, 10, 20, WHITE);        
    DrawText(TextFormat("mvel %0.2f,", mouseVel), 50, 30, 20, WHITE);        
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "raylib");
    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0) -30;
    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);
    xBounds = (Vector2){ScreenToWorld((Vector2){0, 0}).x, ScreenToWorld((Vector2){WIDTH, 0}).x}; 
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 24, 24, 32, 255 });
        Render();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
