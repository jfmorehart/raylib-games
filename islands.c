#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>

#define MAXHULLPOINTS 15


Vector2 IslandPointToWorld(const Island *island, Vector2 objectSpace){
    //translates from object space to world space
    return Vector2Add(island->relativePosition, Vector2Scale(objectSpace, island->scale));
}
int FindNextPoint(int current, const Vector2 *points){
    int indexOfBest = (current + 1) % MAXHULLPOINTS;
    for(int i = 0; i < MAXHULLPOINTS; i++){
        if(i == current) continue;
        Vector2 U = Vector2Subtract(points[indexOfBest], points[current]);
        Vector2 V = Vector2Subtract(points[i], points[current]);
        float cross = (U.x * V.y) - U.y * V.x;
        if(cross < 0 ){
            indexOfBest = i;
        }
    } 
    return indexOfBest;
}
Island CreateIsland(){
    Island is;
    is.scale = 0.5 * (float)rand()/ RAND_MAX;
    is.relativePosition =  RandomWorldPoint();

    Vector2 points[MAXHULLPOINTS];
    int indexOfLowest = 0;
    float lowestX = 999;
    for(int i = 0; i < MAXHULLPOINTS; i++){
        points[i]= RVec(1);
        if(points[i].x < lowestX){
            lowestX = points[i].x;
            indexOfLowest = i;
        }
    }
    Vector2 orderedpoints[MAXHULLPOINTS];
    int currentPointIndex = indexOfLowest;

    is.scale *= 1.1;

    for(int i = 0; i <MAXHULLPOINTS; i++){
        orderedpoints[i] = points[currentPointIndex];
        is.points[i] = orderedpoints[i];
        currentPointIndex = FindNextPoint(currentPointIndex, points);
        is.edges[i] = (Edge){IslandPointToWorld(&is, orderedpoints[i]),  IslandPointToWorld(&is, points[currentPointIndex])};
        if(currentPointIndex == indexOfLowest){
            is.edgeCount = i + 1;
            is.scale *= 0.909;
            return is;
        }
    }
    is.edgeCount =MAXHULLPOINTS;
    is.scale *= 0.909;
    return is;
}

void Render (const Island *island){ 

    for(int i = 0; i < island->edgeCount; i++){

        Vector2 screenPoint0 = WorldToScreen(IslandPointToWorld(island, island->points[0]));
        Vector2 screenPoint1 = WorldToScreen(IslandPointToWorld(island, island->points[i]));
        Vector2 screenPoint2 = WorldToScreen(IslandPointToWorld(island, island->points[(i + 1) % island->edgeCount]));

        DrawLineEx(WorldToScreen(island->edges[i].a), WorldToScreen(island->edges[i].b), 5, WHITE);
        DrawTriangle(screenPoint0, screenPoint1, screenPoint2, BLUE);
    }
}

Hit IslandIntersect(const Island *is, Edge segment){
    Hit current = {false, {0,0}};
    Hit best = {false, {0,0}};
    float bestDistance = 999;
    float currentDist;
    for(int i = 0; i < is->edgeCount; i++){
        current = Intersect(is->edges[i], segment);
        if(current.hit){
            if(!best.hit){
                best = current;
                bestDistance = Vector2DistanceSqr(segment.a, current.hitPosition);
            }else{
                //check which one is closer to segment.a
                currentDist = Vector2DistanceSqr(segment.a, current.hitPosition);
                if(currentDist < bestDistance){
                    best = current;
                    bestDistance = currentDist;
                }
            }
        }
    }
    return best;
}

Hit AllIslandsIntersect(const Island *islands, Edge segment){
    bool hitSmth = false;
    Hit hit = {false, {0, 0}};
    Hit bestHit = hit;
    float bestHitDistance = 999;

    for(int i = 0; i < ISLANDCOUNT; i++){
        hit = IslandIntersect(&islands[i], segment);
        if(hit.hit){
            hitSmth = true;

            float hd = Vector2Distance(hit.hitPosition, segment.a);
            if(hd < bestHitDistance){
                bestHitDistance = hd;
                bestHit = hit;
            }
        }
    }
    if(!hitSmth){
        // DrawLineV(WorldToScreen(segment.b), WorldToScreen(segment.a), GREEN);
    }else{
        // DrawLineV(WorldToScreen(bestHit.hitPosition), WorldToScreen((segment.a)), RED);
    }
    return bestHit;
}