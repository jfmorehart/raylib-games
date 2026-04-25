
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"
#include "pools.h"

#include <stdio.h>

#define BULLET_SPEED 11
int bulletSpeed = 1;

typedef struct{
    PooledObject pObj;
    Vector2 wPos;
    Vector2 direction; //must be normalized!
} Bullet;

extern int bulletCount;
extern int bulletCham;
extern Bullet bulletPool[];

void UpdateAndRenderBullets(Bullet *array, int bulletCount){
    for(int i = 0;i < bulletCount; i++){
        if(!array[i].pObj.active) continue;
        if(Overdue(scaledTime, &array[i].pObj)){
            array[i].pObj.active = false;
        }
        array[i].wPos = Vector2Add(array[i].wPos, Vector2Scale(array[i].direction, BULLET_SPEED * scaledDeltaTime));
        DrawCircleV(WorldToScreen(array[i].wPos), 10, YELLOW);
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

void FireBullet(Vector2 start, Vector2 target){
    printf("fire bullet!");
    //find a bullet, shoot it, mark it ready for updates
    Bullet *b = NextBullet(bulletPool, bulletCount, &bulletCham);
    b->wPos = start;
    Vector2 delta = Vector2Subtract(target, start);
    b->direction = Vector2Normalize(delta);
    b->pObj.active = true;
    b->pObj.lastSpawn = scaledTime;
    b->pObj.lifeTime = 1;
}

