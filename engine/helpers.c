#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"
#include "routines.h"
#include "ships.h"
#include "stb_perlin.h"
#include "game/mapscene.h"
#include "game/map.h"
#include "engine/audio.h"

#include <math.h>


#define STB_PERLIN_IMPLEMENTATION

extern Map currentMap;

float R01(){
    return (float)rand() / RAND_MAX;
}
Vector2 RVec(float scale){
   
    Vector2 rvec =(Vector2){scale * ((float) 2 * rand() / RAND_MAX - 1), scale * ((float) 2 * rand() / RAND_MAX - 1)};
    //printf("rvec: (%f, %f)\n", rvec.x, rvec.y);  
    return rvec;
}
float FBM(float index, float lac, float gain, int oc){
    return stb_perlin_fbm_noise3(index, index * 0.27, index * 0.13, lac, gain, oc);
}
float Perlin(float index){
    return stb_perlin_noise3(index, index * 0.15, index * 0.07, 0, 0, 0);
    // return stb_perlin_noise3(index, 0, 0, 0, 0, 0);
}
Vector2 RVec_Perlin(float index, float wobbleFreq){
    float a = stb_perlin_noise3(index * 413 + 123 + scaledTime * wobbleFreq, 0, 0, 0, 0, 0);
    float b = stb_perlin_noise3(index * 214 + 8976+ scaledTime * wobbleFreq, 0, 0, 0, 0, 0);
    return (Vector2){a, b};
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

float SignedAngle(float a1, float a2){
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
Vector2 GridSnappedVector(Vector2 worldSpace, float gridSize){
    worldSpace.x -= xBounds.x;
    worldSpace.y -= yBounds.y;
    worldSpace.x = round(worldSpace.x / gridSize) * gridSize + xBounds.x;
    worldSpace.y = round(worldSpace.y / gridSize) * gridSize + yBounds.y;
    return worldSpace;
}


bool IsOnScreen(Vector2 worldPoint){
    Vector2 screenPoint = WorldToScreen(worldPoint);
    if(screenPoint.x < 0) return false;
    if(screenPoint.y < 0) return false;
    if(screenPoint.x > WIDTH) return false;
    if(screenPoint.y > HEIGHT) return false;
    return true;
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

float DistanceToShipEdge(const Ship *ship, Vector2 point, float scaleMult){

    //INSIDE TRIANGLE?
    if(IsPointInShip(point, ship, scaleMult)){
        return 0;
    }

    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward);

    float dist = 999;
    float tdist = 0;
    tdist = Vector2Distance(NearestPointOnSegment(leftWing, nose, point), point);
    dist = fminf(dist, tdist);
    tdist = Vector2Distance(NearestPointOnSegment(nose, rightWing, point), point);
    dist = fminf(dist, tdist);
    tdist = Vector2Distance(NearestPointOnSegment(rightWing, tail, point), point);
    dist = fminf(dist, tdist);
    tdist = Vector2Distance(NearestPointOnSegment(tail, leftWing, point), point);
    dist = fminf(dist, tdist);
    return dist;
}

Vector2 NearestPointOnSegment(Vector2 la, Vector2 lb, Vector2 point){
    Vector2 segment = Vector2Subtract(la, lb);
    Vector2 hypo  = Vector2Subtract(point, lb);
    float lenPct = Vector2DotProduct(hypo, segment) / Vector2DotProduct(segment, segment);
    lenPct = Clamp(lenPct, 0, 1);
    return Vector2Add(lb, Vector2Scale(segment, lenPct));
}

bool PointIslandCheck(Vector2 point, const Island *is){

    if(is->pointCount < 3) return false;

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
Island *WhatIslandIsThis(Vector2 wPoint, Island *toCheckArray, int islandLength){
    for(int i = 0; i < islandLength; i++){
        if(PointIslandCheck(wPoint, &toCheckArray[i])){
            return &toCheckArray[i];
        }
    }
    return 0;
}

bool IsPointWithinIslands(Vector2 wPoint){

    for(int i = 0; i < currentMap.islandLength; i++){
        if(PointIslandCheck(wPoint, &currentMap.islands[i])){
            return true;
        }
    }
    return false;
}

void SwitchScenes(SceneName to){ //temp

    EndAllRoutines();
    EndAllNotes();
    bool found = false;
    for(int i = 0 ; i < SCENECOUNT; i++){
        if(scenes[i].name == to){
            scenes[i].RunOnInit();
            found = true;
            break;
        }
    }
    if(!found)TraceLog(LOG_FATAL, "missed Init sequence for scene");
    currentScene = to;
}

Hit RayShipIntersect(Edge ray, Ship *ship, float scaleMult){
    Vector2 forward = VfromAngle(ship->angle);
    // Vector2 forwardNormal = Vector2Normalize(forward);
    forward = Vector2Scale(forward, ship->scale * 5 * scaleMult);

    Vector2 right = {cos(ship->angle + PI * 0.5) * ship->scale * scaleMult, sin(ship->angle +PI * 0.5) * ship->scale * scaleMult};

    Vector2 nose = Vector2Add(ship->wPos, forward);
    Vector2 rightWing = Vector2Add(ship->wPos, right);//Vector2Add(, Vector2Scale(forward, -0.5));
    Vector2 leftWing = Vector2Add(ship->wPos, Vector2Negate(right));//Vector2Add(), Vector2Scale(forward, -0.5));
    Vector2 tail = Vector2Subtract(ship->wPos, forward); 

    float cdist = 999;
    Hit temp;
    Edge ed;
    ed = (Edge){leftWing, nose};
    temp = Intersect(ray, ed);
    if(temp.hit){
        cdist = fminf(cdist, Vector2Distance(ray.a, temp.hitPosition));
    }
    ed = (Edge){nose, rightWing};
    temp = Intersect(ray, ed);
    if(temp.hit){
        cdist = fminf(cdist, Vector2Distance(ray.a, temp.hitPosition));
    }
    ed = (Edge){rightWing, tail};
    temp = Intersect(ray, ed);
    if(temp.hit){
        cdist = fminf(cdist, Vector2Distance(ray.a, temp.hitPosition));
    }
    ed = (Edge){tail, leftWing};
    temp = Intersect(ray, ed);
    if(temp.hit){
        cdist = fminf(cdist, Vector2Distance(ray.a, temp.hitPosition));
    }
    if(cdist < 999){
        return (Hit){true, Vector2Add(ray.a, Vector2Scale(Vector2Normalize(Vector2Subtract(ray.b, ray.a)), cdist))};
    }
    return (Hit){false, Vector2Zero()};
}

Hit RayAllShipsIntersect(Edge ray, Ship * allships, int shipCount, float scaleMult){
    Vector2 chit;
    float nearest = 999;
    Hit temp;
    float thit;
    Ship *besthit;
    for(int i = 0; i < shipCount; i++){
        if(!allships[i].alive) continue;
        if(!allships[i].includedInScene) continue;
        temp = RayShipIntersect(ray, &allships[i], scaleMult);
        if(temp.hit){
            thit = Vector2Distance(temp.hitPosition, ray.a);
            if(thit < nearest){
                besthit = &allships[i];
                nearest = thit;
                chit = temp.hitPosition;
            }
        }
    }
    if(nearest < 999){
        return (Hit){true,chit, besthit};
    }
    return (Hit){false, Vector2Zero()};
}

Hit IntersectIslandsAndShips(Vector2 start, Vector2 angle, Map *m, float scaleMult){
    Edge e = (Edge){start, Vector2Add(start, angle)};
    Hit isl = AllIslandsIntersect(m->islands,e);
    Hit fship = RayAllShipsIntersect(e, m->friendlies, m->fcount, scaleMult);
    Hit eship = RayAllShipsIntersect(e, m->enemies, m->ecount, scaleMult);

    Vector2 cl;
    float nearest = 999;
    float tdist;
    Ship * bestship = 0;
    if(isl.hit){
        tdist = Vector2Distance(start, isl.hitPosition);
        if(tdist < nearest){
            nearest = tdist;
            cl = isl.hitPosition;
            // printf("assign, isle: %f, %f, \n", isl.hitPosition.x, isl.hitPosition.y);
        }
    }
    if(fship.hit){
        tdist = Vector2Distance(start, fship.hitPosition);
        if(tdist < nearest){
            nearest = tdist;
            cl = fship.hitPosition;
            bestship = fship.shipHit;
            // printf("assign, fship: %f, %f, \n", fship.hitPosition.x, fship.hitPosition.y);
        }
    }
    if(eship.hit){
        tdist = Vector2Distance(start, eship.hitPosition);
        if(tdist < nearest){
            nearest = tdist;
            cl = eship.hitPosition;
            bestship = eship.shipHit;
            // printf("assign, eship\n");
        }
    }
    if(nearest < 999){
        return (Hit){true, cl, bestship};
    }
    return (Hit){false, Vector2Zero()};
}