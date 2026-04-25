
#include "bullets.h"
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"
#include "ships.h"

#include <math.h>
#include <stdio.h>

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
        float center = SignedAngle(angleToTarget, ship->angle) * 0 + ship->angle;
        float angle =center + (i * (0.5 * fanAngle / (rays-1))) * alternate;

        float a2T = fabsf(SignedAngle(angleToTarget, angle));

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
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward);

    if(ship->selected){
        DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), BLUE);
        DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), BLUE);
    }else{
        DrawTriangle(WorldToScreen(nose), WorldToScreen(rightWing),WorldToScreen(leftWing), WHITE);
        DrawTriangle(WorldToScreen(tail), WorldToScreen(leftWing), WorldToScreen(rightWing), WHITE);
    }
    
    if(ship->hasMoveTarget && ship->selected){
        DrawLineV(WorldToScreen(ship->wPos), WorldToScreen(ship->moveTargetPosition), WHITE);

    }
}

void SteerShip(Ship *ship, float speedMult){
    //Steer Ship
    if(ship->hasMoveTarget){
        float angle = Path2Target(ship, 4, PI * 0.5, ship->moveTargetPosition);
        float diff = SignedAngle(ship->angle, angle);
        if(diff < -0.01){
            ship->angle -= scaledDeltaTime * SHIPTURN * speedMult;
        }else if(diff >= 0.01){
            ship->angle += scaledDeltaTime * SHIPTURN * speedMult;
        }
        ship->wPos = Vector2Add(ship->wPos, Vector2Scale(VfromAngle(ship->angle), scaledDeltaTime * 0.1 * SHIPSPEED * speedMult));
    }

}


//runs every frame
void ShipCombat(Ship *ship, Ship *targetShipsArray, int arrayLen){

    if(ship->targetShip){
            //check if its still valid
            if(!ship->targetShip->alive){
                ship->targetShip = 0; //null it
                return;
            } 
            if(Vector2DistanceSqr(ship->targetShip->wPos, ship->wPos) > ship->searchRange){
                //out of range, null it
                ship->targetShip = 0;
                return;
            }
            //valid target, see if each battery can fire
            for(int i = 0; i < ship->batteryCount; i++){
                BatteryEngageTarget(ship, &ship->batteries[i], ship->targetShip->wPos);
            }
            
    }else{
        //aquire
        //for now, simple aquire
        for(int i = 0; i < arrayLen; i++){
            if(!targetShipsArray[i].alive)continue;
            if(Vector2DistanceSqr(targetShipsArray[i].wPos, ship->wPos) < ship->searchRange){
                //this is my target!
                printf("target aquire!");
                ship->targetShip = &targetShipsArray[i];

                //valid target, see if each battery can fire
                for(int i = 0; i < ship->batteryCount; i++){
                    BatteryEngageTarget(ship, &ship->batteries[i], ship->targetShip->wPos);
                }
                break;
            }
        }
    }
}

// void ShipCombatOld(Ship *ship, Ship *targetShipsArray, int arrayLen){

//     // DrawCircleV(WorldToScreen(ship->wPos), WorldToPixels(sqrtf(ship->guns[0].range)), WHITE);

//     for(int g = 0; g < ship->battery.gunCount; g++){
//         Gun *gun = &ship->battery.guns[g];
//         if(scaledTime - ship->battery.lastFireTimes[g] > gun->reloadTime){
//             ship->battery.lastFireTimes[g] = scaledTime;
//             if(ship->targetShip){
//                 //check if its still valid
//                 if(!ship->targetShip->active){
//                     ship->targetShip = 0; //null it
//                     return;
//                 } 
//                 if(Vector2DistanceSqr(ship->targetShip->wPos, ship->wPos) > gun->range){
//                     //out of range, null it
//                     ship->targetShip = 0;
//                     return;
//                 }
//                 //valid, in range, fire
//                 FireBullet(ship->wPos, ship->targetShip->wPos, gun);
//             }else{
//                 //aquire
//                 //for now, simple aquire
//                 for(int i = 0; i < arrayLen; i++){
//                     if(!targetShipsArray[i].active)continue;
//                     if(Vector2DistanceSqr(targetShipsArray[i].wPos, ship->wPos) < gun->range){
//                         //this is my target!
//                         printf("target aquire!");
//                         ship->targetShip = &targetShipsArray[i];
//                         //fire
//                         FireBullet(ship->wPos, ship->targetShip->wPos, gun);
//                         break;
//                     }
//                 }
//             }
//         }
//     }
// }