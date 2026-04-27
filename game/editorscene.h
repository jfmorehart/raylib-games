#pragma once

#include "globals.h"
#include "helpers.h"
#include "islands.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

 #ifdef _WIN32
#include <direct.h> //WINDOWS
#elif __APPLE__
#include <sys/stat.h> //MACOS
#endif

typedef enum EditorMode{
    WindIsland,
    PlaceIsland
} EditorMode;

typedef struct Map{
    Island islands[ISLANDCOUNT];
    int islandLength;
}Map;

EditorMode mode;

int currentPointCount;
Island currentIsland;

Map localMap;

// int islandLength;
// Island islands[ISLANDCOUNT];

Vector2 dragOffset;
Island *lastClicked;

Vector2 PointCenter(Island *is){
    Vector2 avg = Vector2Zero();
    for(int i = 0; i < is->pointCount; i++){
        avg = Vector2Add(avg, is->points[i]);
    }
    return Vector2Scale(avg, 1.00 / is->pointCount);
}
void Recenter(Island *is){
    Vector2 c = PointCenter(is);
    for(int i = 0; i < is->pointCount; i++){
        is->points[i] = Vector2Subtract(is->points[i], c);
    }
    is->relativePosition = Vector2Add(is->relativePosition, Vector2Scale(c, is->scale));
}
void RecalculateEdges(Island *is){
    for(int i = 0; i < is->pointCount; i++){
        if(i == is->pointCount - 1){
            is->edges[i] = (Edge){IslandPointToWorld(is, is->points[i]), IslandPointToWorld(is, is->points[0])};
        }else{
            is->edges[i] = (Edge){IslandPointToWorld(is, is->points[i]), IslandPointToWorld(is, is->points[i + 1])};
        }
    }
}

void RecalculateEdgesObjectSpace(Island *is){
    for(int i = 0; i < is->pointCount; i++){
        if(i == is->pointCount - 1){
            is->edges[i] = (Edge){is->points[i], is->points[0]};
        }else{
            is->edges[i] = (Edge){is->points[i], is->points[i + 1]};
        }
    }
}

void ResetCanvas(){
    currentIsland = (Island){0};
    currentIsland.scale = 1;
    currentPointCount = 0;
}


void InitEditorScene(){

    mode = PlaceIsland;

    printf("loading editor scene\n");

    #ifdef _WIN32
        _mkdir("editor");

    #elif __APPLE__
    mkdir("editor", 0777);
    #endif

    FILE *fptr;


    // Create a file
    fptr = fopen("editor/test.campaign", "rb");
    if(!fptr){
        fptr = fopen("test.campaign","w");
        // Close the file
        fclose(fptr);
        ResetCanvas();
        return;
    }
    // Write some text to the file

    Map loadMap;
    fread(&loadMap, sizeof(Map), 1, fptr);

    if(loadMap.islandLength == ISLANDCOUNT){
        localMap = loadMap;
    }

    fclose(fptr);
}

void Redraw(Island *is){
    Recenter(is);
    RecalculateEdges(is);
}

void EditorFrameLoop(){
    ClearBackground(BLACK);

    Vector2 mousePos_ScreenCoords = GetMousePosition();
    mousePos = ScreenToWorld(mousePos_ScreenCoords);

    if(IsKeyPressed(KEY_L)){
        SwitchScenes(MapScene);
    }

    if(IsKeyPressed(KEY_EQUAL)){
        //increase scale
        currentIsland.scale *= 1.1;
        RecalculateEdges(&currentIsland);
        if(lastClicked){
            lastClicked->scale*= 1.1;
            Redraw(lastClicked);
        }
    }
    if(IsKeyPressed(KEY_MINUS)){
        currentIsland.scale *= 0.9;
        RecalculateEdges(&currentIsland);
        if(lastClicked){
            lastClicked->scale*= 0.9;
            Redraw(lastClicked);
        }
    }
    if(IsKeyPressed(KEY_E)){
        mode = WindIsland;
        ResetCanvas();
        if(lastClicked){
            currentPointCount = lastClicked->pointCount;
            currentIsland = *lastClicked;
            Redraw(&currentIsland);
            // *lastClicked = (Island){0};
            // RecalculateEdges(&currentIsland);
        }
    }

    if(IsKeyPressed(KEY_R)){
        ResetCanvas();
    }

    if(IsKeyPressed(KEY_S)){

        Redraw(&currentIsland);

        if(currentPointCount > 2){
            localMap.islands[localMap.islandLength] = currentIsland;
            localMap.islandLength++;
        }

        FILE *fptr = fopen("editor/test.campaign", "wb");
        // Write some text to the file
        fwrite(&localMap, sizeof(Map), 1, fptr);   

        // Close the file
        fclose(fptr); 

        printf("saved to editor folder");
        mode = PlaceIsland;

        ResetCanvas();
    }


    switch (mode) {
        case WindIsland:
            lastClicked = 0;

            RenderObjectSpace(&currentIsland);
            for(int i = 0; i < currentIsland.pointCount; i++){
                DrawCircleV(WorldToScreen(currentIsland.points[i]), 10, RED);
            }

            if(IsMouseButtonPressed(0)){

                if(currentPointCount + 1 < MAXHULLPOINTS){
                    currentIsland.points[currentPointCount] = mousePos;

                    printf("(%f,%f)\n", mousePos.x, mousePos.y);
                    if(currentPointCount > 0){
                        if(currentIsland.edgeCount > 0){
                            currentIsland.edgeCount--;
                        }
                        currentIsland.edges[currentIsland.edgeCount].b = mousePos;
                        currentIsland.edgeCount++;
                        currentIsland.edges[currentIsland.edgeCount].a = mousePos;
                        printf("started next\n");
                        currentIsland.edges[currentIsland.edgeCount].b = currentIsland.edges[0].a;
                        currentIsland.edgeCount++;
                    
                    }else{
                        currentIsland.edges[currentIsland.edgeCount].a = mousePos;
                        // is.edges[is.edgeCount].b = is.points[0];
                        printf("started first\n");
                    }
                    currentPointCount++;
                    currentIsland.pointCount = currentPointCount;
                }   
            }
        break;
        case PlaceIsland:
            for(int i= 0; i < localMap.islandLength; i++){
                Render(&localMap.islands[i]);
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
}