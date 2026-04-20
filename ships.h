
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"

#include <stdio.h>

#define SHIPTURN 2
#define SHIPSPEED 1
#define SHIP_BLEND_MAX 0.15
#define SHIP_SEARCHRANGE 0.35

typedef struct{
    bool team;
    Vector2 wPos;
    float angle;
    float scale;
} Ship;

extern Island island[ISLANDCOUNT];

float Path2Target(const Ship *ship, int rays, float fanAngle, Vector2 target){

    Vector2 d2m = Vector2Subtract(target, ship->wPos);
    float angleToTarget= atan2(d2m.y, d2m.x);
    float tdist = Vector2Length(d2m);
    tdist = Clamp(tdist, 0.03, 0.4);

    float bestAngle = ship->angle + PI;

    float nearestWall_distanceSqr = 999;
    float nearestWall_angle = 0;
    //float angle = ship->angle + (i * (fanAngle / (rays-1))) - (fanAngle * 0.5);

    float best_a2T = PI;

    for(int i = 0; i < rays; i++){
        float alternate = 0;
        if(i % 2 == 0){
            alternate = 1;
        }else{
            alternate = -1;
        }
        //hey chloe
        //(angleToTarget * 0.7) +
        float center = sAngle(angleToTarget, ship->angle) * 0 + ship->angle;
        float angle =center + (i * (0.5 * fanAngle / (rays-1))) * alternate;

        float a2T = fabsf(sAngle(angleToTarget, angle));

        Vector2 delta = VfromAngle(angle);
        Edge segment = {ship->wPos, Vector2Add(Vector2Scale(delta, tdist), ship->wPos)};
        Hit hit = AllIslandsIntersect(island, segment);

        if(hit.hit){

            float d = Vector2DistanceSqr(ship->wPos, hit.hitPosition);
            if(d < nearestWall_distanceSqr){
                nearestWall_distanceSqr = d;
                nearestWall_angle = angle;
            }
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

    if(sAngle(best_a2T, ship->angle) > 0.05 && nearestWall_distanceSqr < SHIP_BLEND_MAX){
        float blendStr = Clamp(1 - nearestWall_distanceSqr/SHIP_BLEND_MAX, 0, 1);
        Vector2 away = VfromAngle(nearestWall_angle + PI);
        Vector2 towards = VfromAngle(bestAngle);
        Vector2 blended = Vector2Add(Vector2Scale(towards,1 -  blendStr), Vector2Scale(away, blendStr));
        return atan2(blended.y,blended.x);
    }
    return bestAngle;
}

void RenderShip(const Ship *ship){
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 3);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale, sin(ship->angle +PI * 0.5) * ship->scale};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward);

    DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), WHITE);
    DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), WHITE);
}

void SteerShip(Ship *ship){
    //Steer Ship
    float angle = Path2Target(ship, 4, PI * 0.5, mousePos);
    float diff = sAngle(ship->angle, angle);
    if(diff < -0.01){
        ship->angle -= deltaTime * SHIPTURN;
    }else if(diff >= 0.01){
        ship->angle += deltaTime * SHIPTURN;
    }
    ship->wPos = Vector2Add(ship->wPos, Vector2Scale(VfromAngle(ship->angle), deltaTime * 0.1 * SHIPSPEED));
}