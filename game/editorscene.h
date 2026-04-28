#pragma once

#include "globals.h"
#include "helpers.h"
#include "islands.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "filesystem.h"

typedef enum EditorMode{
    WindIsland,
    PlaceIsland
} EditorMode;


EditorMode mode;

int currentPointCount;
Island *currentIsland;

Map localMap;

// int islandLength;
// Island islands[ISLANDCOUNT];

Vector2 dragOffset;
Island *lastClicked;
int indexClicked;

Vector2 PointCenter(Island *is){
    if(!is) {
        TraceLog(LOG_FATAL, "island pointer passed was null\n");
    }
    Vector2 avg = Vector2Zero();
    for(int i = 0; i < is->pointCount; i++){
        avg = Vector2Add(avg, is->points[i]);
    }
    return Vector2Scale(avg, 1.00 / is->pointCount);
}
// void SetRelativePositionToAveragePoint(Island *is){
//     if(!is) {
//         TraceLog(LOG_FATAL, "island pointer passed was null\n");
//     }
//     is->relativePosition = PointCenter(is) + Vector2Scale(c, is->scale);
// }
void MoveFromObjectToWorldSpace(Island *is){
    if(!is) {
        TraceLog(LOG_FATAL, "island pointer passed was null\n");
    }
    Vector2 c = PointCenter(is);
    for(int i = 0; i < is->pointCount; i++){
        is->points[i] = Vector2Subtract(is->points[i], c);
    }
    is->relativePosition = Vector2Add(is->relativePosition, Vector2Scale(c, is->scale));
}
void RecalculateEdges(Island *is){
    if(!is) {
        TraceLog(LOG_FATAL, "island pointer passed was null\n");
    }
    for(int i = 0; i < is->pointCount; i++){
        if(i == is->pointCount - 1){
            is->edges[i] = (Edge){IslandPointToWorld(is, is->points[i]), IslandPointToWorld(is, is->points[0])};
        }else{
            is->edges[i] = (Edge){IslandPointToWorld(is, is->points[i]), IslandPointToWorld(is, is->points[i + 1])};
        }
    }
}

void RecalculateEdgesObjectSpace(Island *is){
    if(!is) {
        TraceLog(LOG_FATAL, "island pointer passed was null\n");
    }
    for(int i = 0; i < is->pointCount; i++){
        if(i == is->pointCount - 1){
            is->edges[i] = (Edge){is->points[i], is->points[0]};
        }else{
            is->edges[i] = (Edge){is->points[i], is->points[i + 1]};
        }
    }
}

Island* NextFreeIsland(){
    for(int i = 0; i < ISLANDCOUNT; i++){
        if(localMap.islands[i].pointCount < 3){
            localMap.islands[i] = (Island){0};
            currentIsland = &localMap.islands[i];
            currentIsland->scale = 1;
            if(i >= localMap.islandLength){
                localMap.islandLength = i;
            }
            return &localMap.islands[i];
        }
    }
    TraceLog(LOG_FATAL, "out of free islands!");
    return 0;
}

void ResetCanvas(){
    currentIsland = NextFreeIsland();
    currentPointCount = 0;
}


void InitEditorScene(){

    mode = PlaceIsland;

    Map loadMap = LoadMapFile("editor/test.campaign");

    if(loadMap.islandLength > 0 && loadMap.islandLength <  ISLANDCOUNT){
        //good data
        localMap = loadMap;
    }else{
        //bad data
        localMap = (Map){0};
    }
    GetMapCount();
}

void Redraw(Island *is){
    MoveFromObjectToWorldSpace(is);
    RecalculateEdges(is);
}
void AddNewPoint(Vector2 atPoint){
    if(currentPointCount + 1 < MAXHULLPOINTS){
        currentIsland->points[currentPointCount] = atPoint;

        // printf("(%f,%f)\n", mousePos.x, mousePos.y);
        if(currentPointCount > 0){
            if(currentIsland->edgeCount > 0){
                currentIsland->edgeCount--;
            }
            currentIsland->edges[currentIsland->edgeCount].b = atPoint;
            currentIsland->edgeCount++;
            currentIsland->edges[currentIsland->edgeCount].a = atPoint;
            printf("started next\n");
            currentIsland->edges[currentIsland->edgeCount].b = currentIsland->edges[0].a;
            currentIsland->edgeCount++;
        
        }else{
            currentIsland->edges[currentIsland->edgeCount].a = atPoint;
            // is->edges[is.edgeCount].b = is.points[0];
            printf("started first\n");
        }
        currentPointCount++;
        currentIsland->pointCount = currentPointCount;
    }   
}
float DistanceToPoint(int index, Island *shape, Vector2 point){
    if(index == -1) return 999;
   return Vector2Distance(shape->points[index], point); 
}
int IndexOfNearestPointInShape(Island *is, Vector2 point){
    float dist = 999;
    int index = -1;
    for(int i = 0 ; i < is->pointCount; i++){
        float tdist = DistanceToPoint(i, is, point);
        if(tdist < dist){
            dist = tdist;
            index = i;
        }
    }
    // if(index == -1){
    //     TraceLog(LOG_FATAL, "nearest point didnt work");
    // }
    return index;
}
void EditorFrameLoop(){
    ClearBackground(BLACK);


    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);

    //BACK
    if(IsKeyPressed(KEY_B)){
        SwitchScenes(MapScene);
    }

    //GROW
    if(IsKeyPressed(KEY_EQUAL)){
        //increase scale
        if(currentIsland){
            currentIsland->scale *= 1.1;
            RecalculateEdges(currentIsland);
            if(lastClicked){
                lastClicked->scale*= 1.1;
                Redraw(lastClicked);
            }
        }
    }
    //SHRINK
    if(IsKeyPressed(KEY_MINUS)){
        if(currentIsland) {
            currentIsland->scale *= 0.9;
            RecalculateEdges(currentIsland);
            if(lastClicked){
                lastClicked->scale*= 0.9;
                Redraw(lastClicked);
            }
        }

    }
    //EDIT
    if(IsKeyPressed(KEY_E)){
        mode = WindIsland;
        ResetCanvas();
        if(lastClicked){
            currentPointCount = lastClicked->pointCount;
            currentIsland = lastClicked;
            // Recenter(currentIsland);
            RecalculateEdges(currentIsland);
            // *lastClicked = (Island){0};
            // RecalculateEdges(&currentIsland);
        }else{
            //make a new one
            ResetCanvas();
        }
    }

    //RESET
    if(IsKeyPressed(KEY_R)){
        if(currentIsland){
            currentIsland->pointCount = 0;
        }
        ResetCanvas();
    }


    //QUIT
    if(IsKeyPressed(KEY_Q)){

        FILE* fptr = fopen("test.campaign","w");
        fclose(fptr);

        localMap = (Map){0};

        if(mode == WindIsland){
            ResetCanvas();
        }
    }


    //SAVE
    if(IsKeyPressed(KEY_S)){

        if(currentIsland){

            Redraw(currentIsland); //necessary for sanity tests

            //sanity check -- is it a triangle?
            if(currentIsland->pointCount < 3){
                //kill it and save
                currentIsland->pointCount = 0;
            }else if(!PointIslandCheck(currentIsland->relativePosition, currentIsland)){
                //probably fucked!
                currentIsland->pointCount = 0;
            }
            else{
                //looks good, save it.
                // localMap.islandLength++;
            }
        }
        // if(currentPointCount > 2){
        //     localMap.islands[localMap.islandLength] = currentIsland;
        //     localMap.islandLength++;
        // }

        FILE *fptr = fopen("editor/test.campaign", "wb");
        // Write some text to the file
        fwrite(&localMap, sizeof(Map), 1, fptr);   
        // Close the file
        fclose(fptr); 

        printf("saved to editor folder");
        currentMap = localMap; 
        mode = PlaceIsland;
        ResetCanvas();
    }


    switch (mode) {
        case WindIsland:
            if(!currentIsland){
                TraceLog(LOG_WARNING, "No current island set in wind mode!");
                currentIsland = NextFreeIsland();
            }
            lastClicked = 0;
            for(int i= 0; i < localMap.islandLength; i++){
                // if(&localMap.islands[i] == currentIsland) continue;
                Render(&localMap.islands[i], GRAY);
            }
            if(currentIsland->relativePosition.x == 0.00){
                RenderObjectSpace(currentIsland);
            }

            for(int i = 0; i < currentIsland->pointCount; i++){
                DrawCircleV(WorldToScreen(IslandPointToWorld(currentIsland, currentIsland->points[i])), 10, RED);
            }

            Vector2 islandSpaceMousePos = WorldPointToIsland(currentIsland, mousePos);
            int indexOfNearest = IndexOfNearestPointInShape(currentIsland, islandSpaceMousePos);
            float distToNearestPoint = DistanceToPoint(indexOfNearest, currentIsland, islandSpaceMousePos);

            if(IsMouseButtonPressed(0)){
                if(distToNearestPoint > 0.1){
                    AddNewPoint(islandSpaceMousePos);
                    // SetRelativePositionToAveragePoint(currentIsland);
                    RecalculateEdges(currentIsland);
   
                    // Redraw(currentIsland);

                }else{
                    indexClicked = indexOfNearest;
                }
            }
            if(IsMouseButtonDown(0)){
                if(indexClicked > -1){
                    currentIsland->points[indexClicked] = islandSpaceMousePos;
                    RecalculateEdges(currentIsland);
                }
            }
            if(distToNearestPoint < 0.1){
                 //draw over point
                DrawCircleV(WorldToScreen(IslandPointToWorld(currentIsland, currentIsland->points[indexOfNearest])), 10, GREEN);
            }

            if(IsMouseButtonReleased(0)){
                indexClicked = -1;
            }

        break;
        case PlaceIsland:

            currentIsland = 0;
            for(int i= 0; i < localMap.islandLength; i++){
                Render(&localMap.islands[i], WHITE);
            }
            if(IsMouseButtonPressed(0)){
                Island *click = WhatIslandIsThis(mousePos, localMap.islands, localMap.islandLength);
                if(click){
                    lastClicked = click;
                    dragOffset = Vector2Subtract(click->relativePosition, mousePos);
                }else{
                    lastClicked = 0;
                }
            }
            if(IsMouseButtonReleased(0)){
                dragOffset = Vector2Zero();
            }

            if(IsMouseButtonDown(0)){
                if(lastClicked){
                    lastClicked->relativePosition = Vector2Add(mousePos, dragOffset);
                    RecalculateEdges(lastClicked);
                    printf("dragging %f, %f\n", mousePos.x, mousePos.y);
                }
            }
            if(lastClicked){
                DrawCircleV(WorldToScreen(lastClicked->relativePosition), 50 * lastClicked->scale, BLUE);
            }

        break;
    }
    DrawText(TextFormat("Islands: %d", localMap.islandLength), 400, 400, 20, BLUE);
}