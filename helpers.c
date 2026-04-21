#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"

#include <stdio.h>
#include <string.h>

float R01(){
    return (float)rand() / RAND_MAX;
}
Vector2 RVec(float scale){
   
    Vector2 rvec =(Vector2){scale * ((float) 2 * rand() / RAND_MAX - 1), scale * ((float) 2 * rand() / RAND_MAX - 1)};
    //printf("rvec: (%f, %f)\n", rvec.x, rvec.y);  
    return rvec;
}
Vector2 RandomWorldPoint(){
    Vector2 r01 = (Vector2){(float)rand() / RAND_MAX,  (float) rand() / RAND_MAX};
    r01.x = xBounds.x + (xBounds.y - xBounds.x) * r01.x;
    r01.y = yBounds.x + (yBounds.y - yBounds.x) * r01.y;
    // printf("ro1= %f, %f\n", r01.x, r01.y);
    return r01;
}
Vector2 RandomWorldPointNoIsland(){
    int tries = 0;
    Vector2 point;
    while(tries < 50){
        tries++;
        point = RandomWorldPoint();
        if(!IsPointWithinIslands(point)){
            return point;
        }
    }
    return Vector2Zero();
}

float sAngle(float a1, float a2){
    float diff = a2 - a1;
    if(diff > PI) diff -= 2 * PI;
    if(diff < -PI) diff += 2 * PI;
    return diff;
}

Vector2 VfromAngle(float angle) {return (Vector2){cos(angle), sin(angle)};};

float PixelsToWorld(float pixels) {                     
    return (2 * pixels / HEIGHT) * worldScale;                                      
}                                                                                                                        
   
float WorldToPixels(float world) {                                                                                       
    return (world * HEIGHT * 0.5) / worldScale;                              
}          

Vector2 WorldToScreen(Vector2 worldPos){
    Vector2 screen = Vector2Subtract(worldPos, cameraPosition);
    screen.x = screen.x / worldScale * (HEIGHT * 0.5) + WIDTH * 0.5;
    screen.y = -screen.y / worldScale * (HEIGHT * 0.5) + HEIGHT * 0.5;
    return screen;
}
Vector2 ScreenToWorld(Vector2 screenPos){
    screenPos.x = (screenPos.x - WIDTH * 0.5) / (HEIGHT * 0.5) * worldScale;
    screenPos.y = -(screenPos.y - HEIGHT * 0.5) / (HEIGHT * 0.5) * worldScale;
    screenPos = Vector2Add(screenPos, cameraPosition);
    return screenPos;
}

bool WithinSegment(float xPos, Edge segment){
    if(segment.a.x < xPos){
        if(segment.b.x > xPos){
            return true;
        }
    }else{
        if(segment.b.x < xPos){
             return true;
        }
    }
    return false;
}

Hit Intersect(Edge ab, Edge cd){

    Vector2 ray_ab = Vector2Subtract(ab.b, ab.a);
    float slope_ab= ray_ab.y / ray_ab.x;
    float ab_yinter = ab.a.y - slope_ab * ab.a.x;

    Vector2 ray_cd = Vector2Subtract(cd.b, cd.a);
    float slope_cd= ray_cd.y / ray_cd.x;
    float cd_yinter = cd.a.y - slope_cd * cd.a.x;

    float xIntercept  = (ab_yinter - cd_yinter) / (slope_cd - slope_ab);
    float yIntercept = slope_ab * xIntercept + ab_yinter;

    // printf("testing line %fx + %f\n", slope_ab, ab_yinter);
    // printf("against line %fx +%f\n", slope_cd, cd_yinter);

    bool hit = WithinSegment(xIntercept, ab) && WithinSegment(xIntercept, cd);
    // printf("hit (%f, %f)\n", xIntercept, yIntercept);
    return (Hit){hit,(Vector2){xIntercept, yIntercept}};
}

bool PointIslandCheck(Vector2 point, Island *is){

    float cross = 0;
    bool neg = false;
    Vector2 toPoint;
    Vector2 edgeVec;
    for(int i = 0; i < is->edgeCount; i++){
        toPoint = Vector2Subtract(point, is->edges[i].a);
        edgeVec = Vector2Subtract(is->edges[i].b, is->edges[i].a);
        cross = toPoint.x * edgeVec.y - toPoint.y * edgeVec.x;

        // printf("cross %f\n", cross);
        if(i != 0){
            if((neg && cross > 0) || (!neg && cross < 0)) {
                return false;
            }
        }
        neg = cross < 0;
    }
    return true;
}

bool IsPointWithinIslands(Vector2 wPoint){

    for(int i = 0; i < ISLANDCOUNT; i++){
        if(PointIslandCheck(wPoint, &island[i])){
            return true;
        }
    }
    return false;
}


bool FindRoutineFromName(const char* routineName, Routine **routine){
    for(int i = 0; i < routineCount; i++){
        if(strcmp(routines[i].name,routineName) == 0){
            printf("found %s\n", routineName);
            *routine = &routines[i];
            return true;
        }
    }
    return false;
}

bool RunRoutine(const char* routineName){
    Routine *routine = NULL;
    if(FindRoutineFromName(routineName, &routine)){
        //todo fix time
        printf("found smth");
        if(unscaledTime - routine->startTime < routine->delay) return false;
        printf("activating");
        routine->startTime = unscaledTime;
        routine->isActive = true;
        return true;
    }
    return false;
}
