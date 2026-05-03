
#include "globals.h"
#include "helpers.h"
#include "raylib.h"
#include "pools.h"
#include "vfx.h"
#include <raylib.h>
#include "rlgl.h"
#include "raymath.h"
#include "map.h"

#include <math.h>
#include <stdio.h>

extern int smokeCount;
extern int smokeCham;
extern Smoke smokePool[];

extern int splashCount;
extern int splashCham;
extern Smoke splashPool[];

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
    if(*cham >= poolSize) *cham = 0;
    return ret;
}


void FireSplash(Vector2 position, float radius){
    Smoke *smoke = NextSmoke(splashPool, splashCount, &splashCham);
    smoke->wPos = position;
    smoke->radius = radius;
    smoke->pObj.active = true;
    smoke->pObj.lifeTime = 0.4;
    smoke->pObj.lastSpawn = scaledTime;
}

void FireSmoke(Vector2 position, float radius){
    Smoke *smoke = NextSmoke(smokePool, smokeCount, &smokeCham);
    smoke->wPos = position;
    smoke->radius = radius;
    smoke->pObj.active = true;
    smoke->pObj.lifeTime = 0.4;
    smoke->pObj.lastSpawn = scaledTime;
}

void UpdateAndRenderBlobs(Smoke *pool, int count){
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_QUADS);                                                       
                         
    for(int i = 0; i < count; i++){
        if(!pool[i].pObj.active) continue;
        float pct = LifePct(scaledTime, &pool[i].pObj);
        if(pct > 1){
            pool[i].pObj.active = false;
            continue;
        }
        float rad = (1 - pct) * pool[i].radius;

        Vector2 c = WorldToScreen(pool[i].wPos);
        float r = rad;                     
        rlTexCoord2f(0, 0);                                                                                                
        rlVertex2f(c.x - r, c.y - r);                                                                                        
                                                                    
        // bottom-left                                                                                                     
        rlTexCoord2f(0, 1);                         
        rlVertex2f(c.x - r, c.y + r);                                 
                                                                                                                            
        // bottom-right
        rlTexCoord2f(1, 1);                                                                                                
        rlVertex2f(c.x + r, c.y + r);                 
                                                                    
        // top-right                                                                                                       
        rlTexCoord2f(1, 0);
        rlVertex2f(c.x + r, c.y - r);    
        // DrawCircleV(WorldToScreen(pool[i].wPos), rad, YELLOW);
    }
    rlEnd();                                                        
    rlSetTexture(0); 
}

#define BEAMSEGMENTS_MAX 10
void DrawBeam(Vector2 start, Vector2 target, float angle, int beamSegments, float beamLength, Map *m, float scaleMult){
    Hit hits[BEAMSEGMENTS_MAX] = {0};

    Vector2 beamDir = Vector2Subtract(target, start);
    float startingAngle = atan2(beamDir.y, beamDir.x) - angle * 0.5;

    float apb = angle / (float)beamSegments;

    
    for(int i = 0; i < beamSegments; i++){
        Vector2 dir = VfromAngle(startingAngle + apb * i);
        dir = Vector2Scale(Vector2Normalize(dir), beamLength);
        hits[i] = IntersectIslandsAndShips(start, dir, m, scaleMult);

        if(!hits[i].hit || Vector2Length(hits[i].hitPosition) < 0.001){
            printf("miss, setting to:(%f, %f)\n",  Vector2Add(start, dir).x, Vector2Add(start, dir).y);
            hits[i].hitPosition = Vector2Add(start, dir);
        }else{
            printf("hit :(%f, %f)\n",  hits[i].hitPosition.x, hits[i].hitPosition.y);
        }

    }

    for(int i = 1; i < beamSegments; i++){


        Vector2 c = WorldToScreen(start);               
        Vector2 h1 = WorldToScreen(hits[i - 1].hitPosition); 
        float l = Vector2Distance(start, hits[i - 1].hitPosition) / beamLength;
        float l2 = Vector2Distance(start, hits[i].hitPosition) / beamLength;
        Vector2 h2 =  WorldToScreen(hits[i].hitPosition);      
    
        //start
        float uvval = (float)i / beamSegments;
        rlTexCoord2f(1, uvval);         
        rlVertex2f(c.x, c.y);
                                                                    
        // left                                                                                                  
        rlTexCoord2f(1 - l, uvval);                         
        rlVertex2f(h1.x, h1.y);                                 
                                                                                                                            
        // right
        rlTexCoord2f(1 - l2, uvval);                                                                                                
        rlVertex2f(h2.x, h2.y);               

        // DrawTriangle(WorldToScreen(start), WorldToScreen(hits[i - 1].hitPosition), WorldToScreen(hits[i].hitPosition), YELLOW);
        // DrawLineEx(WorldToScreen(start), WorldToScreen(hits[i].hitPosition), 3, WHITE);
    }
}
