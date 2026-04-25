
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

Gun FiveInch = {0.3, 2, 0.01, 0.05, 10};
Gun EightInch = {0.65, 4, 0.013, 0.04, 10};
Gun SixteenInch = {1.2, 8, 0.02, 0.03, 10};



extern int bulletCount;
extern int bulletCham;
extern Bullet bulletPool[];

void DamageShips(Vector2 position, float radius, Ship *enemyShips, int count, int damage){
    for(int i = 0; i < count; i++){
        if(Vector2Distance(enemyShips[i].wPos, position) < radius){
            //hit
            printf("HIT");
            enemyShips[i].health -= damage;
            if(enemyShips[i].health < 0){
                enemyShips[i].alive = false;
            }
        }
    }
}

void UpdateAndRenderBullets(Bullet *array, int bulletCount, Ship *canDamageArray, int canDamageLength){
    for(int i = 0;i < bulletCount; i++){
        if(!array[i].pObj.active) continue;
          
        //lifetime handling
        float pct = LifePct(scaledTime, &array[i].pObj);
        if(pct > 1){

            //explosion!!
            array[i].pObj.active = false;
            FireSmoke(array[i].tPos, WorldToPixels(array[i].expRadius));
            //damage calc
            DamageShips(array[i].tPos, array[i].expRadius, canDamageArray, canDamageLength, array[i].damage);
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
    
    target = Vector2Add(target, Vector2Scale(RVec(btype.spread), 0.5 * Vector2Distance(start, target) / btype.range));
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

bool CanBatterySeeThis(Vector2 batteryPosition, float batteryAngle, Battery *battery, Vector2 target, bool drawRanges){
    Vector2 delta = Vector2Subtract(target, batteryPosition);
    float distance = Vector2Length(delta);
    float theta = SignedAngle(batteryAngle, atan2f(delta.y, delta.x));

    if(drawRanges){
        Vector2 left = Vector2Add(Vector2Scale(VfromAngle(batteryAngle - battery->traverseAmount * DEG2RAD * 0.5), battery->BatteryType.range), batteryPosition);
        Vector2 right = Vector2Add(Vector2Scale(VfromAngle(batteryAngle +  battery->traverseAmount * DEG2RAD * 0.5), battery->BatteryType.range), batteryPosition);

        DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(left), 10, GREEN);
        DrawLineEx(WorldToScreen(batteryPosition), WorldToScreen(right), 10, RED);
        DrawLineEx(WorldToScreen(left), WorldToScreen(right), 10, RED);
    }

    if(distance > battery->BatteryType.range) {
        printf("out of range");
        return false;
    }

    printf("angle = %f\n", theta);
    if(fabs(theta) >  battery->traverseAmount * DEG2RAD) return false;

    return true;
}
//runs every frame that we have a valid target
void BatteryEngageTarget(const Ship *ship, Battery *battery, Vector2 target){

    //CHECK IF BATTERY CAN ENGAGE
    //Range, Angle
    float batteryAngle = atan2f(battery->batteryForward.y, battery->batteryForward.x);
    if(!CanBatterySeeThis(ship->wPos, ship->angle + batteryAngle, battery, target, ship->selected)){
        return;
    }

    for(int g = 0; g < battery->gunCount; g++){

        //fire if reloaded
        if(scaledTime - battery->lastFireTimes[g] > battery->BatteryType.reloadTime){
            //shoot
            battery->lastFireTimes[g] = scaledTime + (R01() - 0.5) * battery->BatteryType.reloadTime * 0.1;
            FireBullet(ship->wPos, target, battery->BatteryType);
        }
    }
}