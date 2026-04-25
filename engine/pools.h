#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "globals.h"

typedef struct PooledObject{
    bool active;
    float lifeTime;
    float lastSpawn;
    Vector2 wPos;
    // void (*onUpdate) (struct PooledObject *obj);
    // void (*onComplete) (struct PooledObject *obj);
} PooledObject;

bool Overdue(float time, const PooledObject *obj){
    return(time - obj->lastSpawn) > obj->lifeTime;
}
float LifePct(float time, const PooledObject *obj){
    return (time - obj->lastSpawn) / obj->lifeTime;
}


PooledObject *Next(PooledObject *array, int poolSize, int *cham){
    int tries = 0;
    while (array[*cham].active) {
        if(tries >= poolSize) {
            TraceLog(LOG_FATAL, "bulletpool is full!");
            return 0;
        }
        (*cham)++;
        tries++;
        if(*cham >= poolSize) *cham = 0;
    }
    return &array[*cham];
}

