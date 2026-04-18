#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include <stdio.h>

Vector2 RVec(float scale){
   
    Vector2 rvec =(Vector2){scale * ((float) 2 * rand() / RAND_MAX - 1), scale * ((float) 2 * rand() / RAND_MAX - 1)};
    //printf("rvec: (%f, %f)\n", rvec.x, rvec.y);  
    return rvec;
}

float PixelsToWorld(float pixels) {                     
    return 2 *  pixels / HEIGHT;                                      
}                                                                                                                        
   
float WorldToPixels(float world) {                                                                                       
    return world * HEIGHT * 0.5;                              
}          

Vector2 WorldToScreen(Vector2 worldPos){
    return (Vector2){(0.5 * worldPos.x + 0.5) * HEIGHT, (0.5 * -worldPos.y + 0.5) * HEIGHT};
}
Vector2 ScreenToWorld(Vector2 screenPos){
    screenPos = Vector2Scale(Vector2SubtractValue(Vector2Scale(screenPos, (float)1 / HEIGHT), 0.5), 2);
    screenPos.y *= -1;
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
    printf("hit (%f, %f)\n", xIntercept, yIntercept);
    return (Hit){hit,(Vector2){xIntercept, yIntercept}};
}