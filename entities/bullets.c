
#include "helpers.h"
#include "raylib.h"
#include "raymath.h"
#include "globals.h"
#include "pools.h"
#include "vfx.h"
#include "ships.h"
#include "bullets.h"

#include <math.h>
#include <stdio.h>

/*typedef struct {
    float range;
    float reloadTime;
    float explosionRadius;
    float spread;
    float damage;
} Gun;*/

Gun FiveInch = {0.3, 2, 0.01, 0.04, 10};
Gun EightInch = {0.65, 4, 0.013, 0, 10};
Gun SixteenInch = {1.2, 8, 0.02, 0, 10};



extern int bulletCount;
extern int bulletCham;
extern Bullet bulletPool[];

bool DamageShips(Vector2 position, float radius, Ship *enemyShips, int count, int damage){
    int h = 0;
    for(int i = 0; i < count; i++){
        if(Vector2Distance(enemyShips[i].wPos, position) < radius){
            //hit
            h++;
            enemyShips[i].health -= damage;
            if(enemyShips[i].health < 0){
                enemyShips[i].alive = false;
            }
        }
    }
    return h;
}

void UpdateAndRenderBullets(Bullet *array, int bulletCount, Ship *canDamageArray, int canDamageLength){
    for(int i = 0;i < bulletCount; i++){
        if(!array[i].pObj.active) continue;
          
        //lifetime handling
        float pct = LifePct(scaledTime, &array[i].pObj);
        if(pct > 1){

            //explosion!!
            array[i].pObj.active = false;

            //damage calca
            if(DamageShips(array[i].tPos, array[i].expRadius, canDamageArray, canDamageLength, array[i].damage)){
                  FireSmoke(array[i].tPos, WorldToPixels(array[i].expRadius)); 
            }else{
                FireSplash(array[i].tPos, WorldToPixels(array[i].expRadius));
            }
           
            continue;
        }
        //slows down as it travels
        float lerpTime = powf(pct, 0.8);

        //roll triangle
        Vector2 center = Vector2Lerp(array[i].wPos, array[i].tPos, lerpTime);
        Vector2 toright = Vector2Add(center, array[i].right);
        center = Vector2Subtract(center, array[i].right);
        //add some drag to the tail
        Vector2 tail = Vector2Lerp(array[i].wPos, array[i].tPos, fmax(lerpTime * 0.7, lerpTime - 0.7));
        DrawTriangle(WorldToScreen(center),WorldToScreen(toright), WorldToScreen(tail), YELLOW);

    }
}
//copy and paste of Pools Next
Bullet *NextBullet(Bullet *array, int poolSize, int *cham){
    int tries = 0;
    while (array[*cham].pObj.active) {
        if(tries >= poolSize) {
            TraceLog(LOG_FATAL, "bulletpool is full!");
            return 0;
        }
        (*cham)++;
        tries++;
        if(*cham >= poolSize) *cham = 0;
    }
    Bullet *bul = &array[*cham];
    (*cham)++;
    if(*cham >= poolSize) *cham = 0;
    return bul;
}

void FireBullet(Vector2 start, Vector2 target, Gun btype){
    //gun set to reload
    // btype->lastShot = scaledTime + (R01()  - 0.5) * 0.1;

    //find a bullet, mark it ready for updates
    Bullet *b = NextBullet(bulletPool, bulletCount, &bulletCham);

    //basics
    b->wPos = start;
    b->expRadius = btype.explosionRadius + (R01() - 0.5) * 0.25 * btype.explosionRadius;
    b-> damage = btype.damage;
    
    target = Vector2Add(target, Vector2Scale(RVec(btype.spread), Vector2Distance(start, target) / btype.range));
    Vector2 delta = Vector2Subtract(target, start);
    b->tPos = target;
    b->pObj.active = true;
    b->pObj.lastSpawn = scaledTime;

    //scale lifetime so that it can just lerp to target

    b->pObj.lifeTime = Vector2Length(delta) / BULLET_SPEED;

    //generate a right hand vector for use with the triangles
    float theta = atan2f(delta.y, delta.x);
    theta += PI / 2;
    b->right = Vector2Scale((Vector2){cos(theta), sin(theta)}, 0.002);

    //gunsmoke
    FireSmoke(Vector2Add(start, Vector2Scale(Vector2Normalize(delta), 0.01)), WorldToPixels(b->expRadius * 0.5));
}

bool CanBatterySeeThis(Vector2 batteryPosition, float batteryAngle, Battery *battery, Vector2 target){
    Vector2 delta = Vector2Subtract(target, batteryPosition);
    float distance = Vector2Length(delta);
    float theta = SignedAngle(batteryAngle, atan2f(delta.y, delta.x));

    if(distance > battery->BatteryType.range) {
        return false;
    }

    if(fabs(theta) > battery->traverseAmount * DEG2RAD * 0.5){
        return false;
    }
    return true;
}
//runs every frame that we have a valid target
void BatteryEngageTarget(Vector2 batteryPosition, Battery *battery, Vector2 target){

    //CHECK IF BATTERY CAN ENGAGE (moved to Update)
    //Range, Angle
    // float batteryAngle = atan2f(battery->batteryForward.y, battery->batteryForward.x) + ship->angle;
    // if(!CanBatterySeeThis(ship->wPos, batteryAngle, battery, target, ship->selected)){
    //     return;
    // }
    Gun btype = battery->BatteryType;
    Vector2 rvec = RVec_Perlin(battery->_r_index, 1);
    float innaccuracy = battery->batterySpread * Vector2Distance(batteryPosition, target);
    float accuracy = (btype.range * ((battery->timesTargeted + 1) * 0.3));
    Vector2 spreadTarget = Vector2Add(target, Vector2Scale(rvec,  fminf(innaccuracy / accuracy, 0.3)));
    // printf("%f\n", Vector2Distance(batteryPosition, target) / (btype.range * battery->timesTargeted));

    for(int g = 0; g < battery->gunCount; g++){

        //fire if reloaded
        if(scaledTime - battery->lastFireTimes[g] > battery->BatteryType.reloadTime){
            //shoot
            battery->lastFireTimes[g] = scaledTime + (R01() - 0.5) * battery->BatteryType.reloadTime * 0.1;

            // Vector2 delta = Vector2Subtract(target, batteryPosition);
            // Vector2 perp = Vector2Scale(VfromAngle(atan2f(delta.y, delta.x) + PI * 0.5), 0.02);
            
            // DrawTriangle(WorldToScreen(batteryPosition), WorldToScreen(Vector2Add(target, perp)),  WorldToScreen(Vector2Add(target, perp)), WHITE);
            // DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(Vector2Subtract(target, perp)), 20, WHITE);
            FireBullet(batteryPosition, spreadTarget, battery->BatteryType);
            battery->timesTargeted++;
        }
    }
}

//heavy
Ship *BatteryAquireTarget(const Ship *ship, Ship *targetShipsArray, int arrayLen, Battery *battery, Vector2 batteryPosition){
    for(int i = 0; i < arrayLen; i++){
        if(!targetShipsArray[i].alive)continue;
        float batteryAngle = atan2f(battery->batteryForward.y, battery->batteryForward.x) + ship->angle;
        if(CanBatterySeeThis(batteryPosition, batteryAngle,  battery, targetShipsArray[i].wPos)){
            return &targetShipsArray[i];
        }
    }
    return 0;
}

void BatteryUpdate(const Ship *ship, Ship *targetShips, int arrayLen, Battery *battery){

    //ranging lines
    float batteryAngle = atan2f(battery->batteryForward.y, battery->batteryForward.x) + ship->angle;
    Vector2 batteryPosition = Vector2Add(ship->wPos, Vector2Scale(VfromAngle(ship->angle), battery->batteryOffset_Y * ship->scale));
    if(ship->selected){
        Vector2 left = Vector2Add(Vector2Scale(VfromAngle(batteryAngle - battery->traverseAmount * DEG2RAD * 0.5), battery->BatteryType.range), batteryPosition);
        Vector2 right = Vector2Add(Vector2Scale(VfromAngle(batteryAngle +  battery->traverseAmount * DEG2RAD * 0.5), battery->BatteryType.range), batteryPosition);

        DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(left), 10, GREEN);
        DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(right), 10, RED);
        DrawLineEx(WorldToScreen(left), WorldToScreen(right), 10, RED);
    }

    if(battery->shipTarget){
        Gun btype = battery->BatteryType;
        Vector2 rvec = RVec_Perlin(battery->_r_index, 1);
        float innaccuracy = battery->batterySpread * Vector2Distance(batteryPosition, battery->shipTarget->wPos);
        float accuracy = (btype.range * ((battery->timesTargeted + 1) * 0.3));
        Vector2 spreadTarget = Vector2Add(battery->shipTarget->wPos, Vector2Scale(rvec,  fmin(innaccuracy / accuracy, 0.3)));
        DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(spreadTarget), 10, GREEN);
    }
    
    //valid target?
    if(battery->shipTarget){

        //alive?
        if(!battery->shipTarget->alive){
            //schedule retarget
            battery->shipTarget = 0;
            battery->timesTargeted = 0;
            return;
        } 
      
        //CHECK IF BATTERY CAN ENGAGE 
        //Range, Angle
        if(CanBatterySeeThis(batteryPosition, batteryAngle, battery, battery->shipTarget->wPos)){
            //valid target, see if each battery can fire
            // battery->timesTargeted++;
            // if(battery->timesTargeted > 4) battery->timesTargeted = 4;
            // printf("firing on %p, time %d, spread = ", battery->shipTarget, battery->timesTargeted);
            BatteryEngageTarget(batteryPosition, battery, battery->shipTarget->wPos);
        }else{
            //schedule retarget
            battery->shipTarget = 0;
            battery->timesTargeted = 0;
        }
    }else{

        //check if we can find new target
        if(scaledTime - battery->lastSearch > battery->BatteryType.reloadTime){
        //search
            battery->lastSearch = scaledTime + (R01() - 0.5) * battery->searchCooldown* 0.1;
            battery->shipTarget = BatteryAquireTarget(ship, targetShips, arrayLen, battery, batteryPosition);
        }
    }
}