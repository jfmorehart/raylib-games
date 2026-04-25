
#include "globals.h"
#include "helpers.h"
#include "raylib.h"
#include "pools.h"
#include "vfx.h"

#include <math.h>
#include <stdio.h>

extern int smokeCount;
extern int smokeCham;
extern Smoke smokePool[];

Smoke *NextSmoke(Smoke *array, int poolSize, int *cham){
    int tries = 0;
    while (array[*cham].pObj.active) {
        if(tries >= poolSize) {
            TraceLog(LOG_FATAL, "smokepool is full!");
            return 0;
        }
        (*cham)++;
        tries++;
        if(*cham >= poolSize) *cham = 0;
    }
    Smoke *ret =  &array[*cham];
    (*cham)++;
    return ret;
}

void FireSmoke(Vector2 position, float radius){
    Smoke *smoke = NextSmoke(smokePool, smokeCount, &smokeCham);
    smoke->wPos = position;
    smoke->radius = radius;
    smoke->pObj.active = true;
    smoke->pObj.lifeTime = 0.4;
    smoke->pObj.lastSpawn = scaledTime;
}

void UpdateAndRenderSmokes(){
    for(int i = 0; i < smokeCount; i++){
        if(!smokePool[i].pObj.active) continue;
        float pct = LifePct(scaledTime, &smokePool[i].pObj);
        if(pct > 1){
            smokePool[i].pObj.active = false;
            continue;
        }
        float rad = (1 - pct) * smokePool[i].radius;
        DrawCircleV(WorldToScreen(smokePool[i].wPos), rad, YELLOW);
    }
}
