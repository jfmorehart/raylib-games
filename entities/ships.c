
#include "bullets.h"
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"
#include "ships.h"
#include "map.h"
#include "rlgl.h"

#include <math.h>
#include <stdio.h>


bool IsPointInShip(Vector2 worldSpace, const Ship *ship, float scaleMult){
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward); 

    if(CheckCollisionPointTriangle(worldSpace, nose, rightWing, leftWing)) return true;
    if(CheckCollisionPointTriangle(worldSpace, rightWing, tail, leftWing)) return true;
    return false;
}

float Path2Target(const Ship *ship, int rays, float fanAngle, Vector2 target, Island *obstacles){

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
        float center = SignedAngle(angleToTarget, ship->angle) * 0 + ship->angle;
        float angle =center + (i * (0.5 * fanAngle / (rays-1))) * alternate;

        float a2T = fabsf(SignedAngle(angleToTarget, angle));

        Vector2 delta = VfromAngle(angle);
        Edge segment = {ship->wPos, Vector2Add(Vector2Scale(delta, tdist), ship->wPos)};
        Hit hit = AllIslandsIntersect(obstacles, segment);

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

    if(SignedAngle(best_a2T, ship->angle) > 0.05 && nearestWall_distanceSqr < SHIP_BLEND_MAX){
        float blendStr = Clamp(1 - nearestWall_distanceSqr/SHIP_BLEND_MAX, 0, 1);
        Vector2 away = VfromAngle(nearestWall_angle + PI);
        Vector2 towards = VfromAngle(bestAngle);
        Vector2 blended = Vector2Add(Vector2Scale(towards,1 -  blendStr), Vector2Scale(away, blendStr));
        return atan2(blended.y,blended.x);
    }
    return bestAngle;
}

void RenderShip(const Ship *ship, float scaleMult){ 
    Vector2 snPos = GridSnappedVector(ship->wPos, 0.1);
    DrawCircleV(WorldToScreen(snPos), 200 * ship->scale * scaleMult, WHITE);
    // Vector2 forward = VfromAngle(ship->angle);
    // // Vector2 forwardNormal = Vector2Normalize(forward);
    // forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    // Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    // Vector2 nose = Vector2Add(ship->wPos, forward);
    // Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    // Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    // Vector2 tail = Vector2Subtract(ship->wPos, forward);

    // if(ship->selected){
    //     DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), BLUE);
    //     DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), BLUE);
    // }else{
    //     DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), WHITE);
    //     DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), WHITE);
    // }
    
    if(ship->hasMoveTarget && ship->selected){
        DrawLineV(WorldToScreen(ship->wPos), WorldToScreen(ship->moveTargetPosition), WHITE);
    }
}

void RenderShipColor(const Ship *ship, float scaleMult, Vector3 color){ 
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward);
                            
    Vector2 no = WorldToScreen(nose);
    Vector2 le = WorldToScreen(leftWing);
    Vector2 ri = WorldToScreen(rightWing);
    Vector2 ta = WorldToScreen(tail);
    //nose

    rlColor3f(color.x, color.y, color.z);

    rlTexCoord2f(0.5, 1);                                                                                                
    rlVertex2f(no.x, no.y);                                                                                        
       
    // right
    rlTexCoord2f(1, 0.5);                                                                                                
    rlVertex2f(ri.x, ri.y);                 

    // left                                                                                                
    rlTexCoord2f(0, 0.5);                         
    rlVertex2f(le.x, le.y);            
           
    // right
    rlTexCoord2f(1, 0.5);                                                                                                
    rlVertex2f(ri.x, ri.y);                 

    // tail                                                                                                     
    rlTexCoord2f(0.5, 0);
    rlVertex2f(ta.x, ta.y);  
    
    // left                                                                                                
    rlTexCoord2f(0, 0.5);                         
    rlVertex2f(le.x, le.y);        
                         

    // if(ship->selected){
    //     DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), BLUE);
    //     DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), BLUE);
    // }else{
    //     DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), WHITE);
    //     DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), WHITE);
    // }
    if(ship->hasMoveTarget && ship->selected){
        DrawLineV(WorldToScreen(ship->wPos), WorldToScreen(ship->moveTargetPosition), WHITE);
    } 
}

void SteerShip(Ship *ship, bool avoidIslands, Island *islandsToAvoid){
    //Steer Ship
    if(ship->hasMoveTarget){
        float angle;
        if(avoidIslands){
            angle = Path2Target(ship, 4, PI * 0.5, ship->moveTargetPosition, islandsToAvoid);
        }else{
            Vector2 delta =  Vector2Subtract(ship->moveTargetPosition, ship->wPos);
            angle = atan2f(delta.y, delta.x);
        }

        float diff = SignedAngle(ship->angle, angle);
        if(diff < -0.01){
            ship->angle -= scaledDeltaTime * SHIPTURN;
        }else if(diff >= 0.01){
            ship->angle += scaledDeltaTime * SHIPTURN;
        }
        ship->wPos = Vector2Add(ship->wPos, Vector2Scale(VfromAngle(ship->angle), scaledDeltaTime * SHIPSPEED));
    }
}
void SteerShipBattle(Ship *ship, bool avoidIslands, Island *islandsToAvoid){
    //Steer Ship
    if(ship->hasMoveTarget){
        float angle;
        if(avoidIslands){
            angle = Path2Target(ship, 4, PI * 0.5, ship->moveTargetPosition, islandsToAvoid);
        }else{
            Vector2 delta =  Vector2Subtract(ship->moveTargetPosition, ship->wPos);
            angle = atan2f(delta.y, delta.x);
        }

        float diff = SignedAngle(ship->angle, angle);
        if(diff < -0.01){
            ship->angle -= scaledDeltaTime * SHIPTURN * BATTLESCENE_SPEEDMULT;
        }else if(diff >= 0.01){
            ship->angle += scaledDeltaTime * SHIPTURN * BATTLESCENE_SPEEDMULT;
        }
        ship->wPos = Vector2Add(ship->wPos, Vector2Scale(VfromAngle(ship->angle), scaledDeltaTime * SHIPSPEED * BATTLESCENE_SPEEDMULT));
    }
}

//runs every frame
void ShipCombat(Ship *ship, Ship *targetShipsArray, int arrayLen){
    for(int i = 0; i < ship->batteryCount; i++){
        BatteryUpdate(ship, targetShipsArray, arrayLen, &ship->batteries[i]);
    }
}
