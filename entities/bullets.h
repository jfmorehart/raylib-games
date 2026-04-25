
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"
#include "pools.h"
#include "vfx.h"

#include <math.h>
#include <stdio.h>

#define BULLET_SPEED 0.1

typedef struct {
    float range;
    float reloadTime;
    float explosionRadius;
    float spread;

    float lastShot;
} Gun;
Gun FiveInch = {0.18, 2, 0.1, 0.05};
Gun EightInch = {0.25, 4, 0.13, 0.04};
Gun SixteenInch = {0.4, 8, 0.2, 0.03};


typedef struct{
    PooledObject pObj;
    Vector2 wPos;
    Vector2 tPos;
    Vector2 right; //used for triangle
} Bullet;

extern int bulletCount;
extern int bulletCham;
extern Bullet bulletPool[];

void UpdateAndRenderBullets(Bullet *array, int bulletCount){
    for(int i = 0;i < bulletCount; i++){
        if(!array[i].pObj.active) continue;
          
        float pct = LifePct(scaledTime, &array[i].pObj);
        if(pct > 1){
            array[i].pObj.active = false;
            FireSmoke(array[i].tPos, R01() * 5 + 8);
            continue;
        }
        // pct * array[i].invDistance * BULLET_SPEED);
        float lerpTime = powf(pct, 0.8);
        Vector2 center = Vector2Lerp(array[i].wPos, array[i].tPos, lerpTime);
        Vector2 toright = Vector2Add(center, array[i].right);
        center = Vector2Subtract(center, array[i].right);//, Vector2 v2)
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

void FireBullet(Vector2 start, Vector2 target, Gun *btype){
    btype->lastShot = scaledTime + (R01()  - 0.5) * 0.1;
    //find a bullet, shoot it, mark it ready for updates
    Bullet *b = NextBullet(bulletPool, bulletCount, &bulletCham);
    b->wPos = start;
    target = Vector2Add(target, RVec(btype->spread));
    Vector2 delta = Vector2Subtract(target, start);
    b->tPos = target;
    b->pObj.active = true;
    b->pObj.lastSpawn = scaledTime;
    b->pObj.lifeTime = Vector2Length(delta) / BULLET_SPEED;
    float theta = atan2f(delta.y, delta.x);
    theta += PI / 2;
    b->right = Vector2Scale((Vector2){cos(theta), sin(theta)}, 0.002);

    FireSmoke(Vector2Add(start, Vector2Scale(Vector2Normalize(delta), 0.01)), R01() * 3 + 4);
    // b->invDistance = 1 / Vector2Length(delta);
}