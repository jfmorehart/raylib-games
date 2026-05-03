#pragma once

#include "globals.h"
#include "helpers.h"
#include "islands.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "filesystem.h"
#include "UI.h"
#include "shiploadouts.h"
#include <string.h>

typedef enum EditorMode{
    WindIsland,
    PlaceIsland
} EditorMode;

extern Map currentMap;

EditorMode mode;

int currentPointCount;
Island *currentIsland;

Map localMap;

Vector2 dragOffset;
Island *lastClicked;
int indexClicked;

extern Ship destroyerShip;

StringArray allFiles;

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
    timeScale = 0;

    Map loadMap = LoadMapFile("test.situ");

    if(loadMap.islandLength > 0 && loadMap.islandLength <  ISLANDCOUNT){
        //good data
        localMap = loadMap;
    }else{
        //bad data
        localMap = (Map){0};
        AssignName(&localMap, "test.situ");
    }
    allFiles = GetMapNames();
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
void GenericInput(){
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
        if(fptr){
            fclose(fptr);
        }

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

                //TODO figure out when to increment islandlength
            }
        }
        // if(currentPointCount > 2){
        //     localMap.islands[localMap.islandLength] = currentIsland;
        //     localMap.islandLength++;
        // }
        if(localMap.filename[0] == 0) return;
        char fullstring[30] = "editor/";
        strcat(fullstring, localMap.filename);
        
        FILE *fptr = fopen(fullstring, "wb");
        if(fptr){
            // Write some text to the file
            fwrite(&localMap, sizeof(Map), 1, fptr);   
            // Close the file
            fclose(fptr); 

            printf("saved to editor folder");
            currentMap = localMap; 
            printf("named = %s\n", currentMap.filename);
            mode = PlaceIsland;
            ResetCanvas();
        }
    }

}

void EditIslandMode(){
            if(!currentIsland){
                TraceLog(LOG_WARNING, "No current island set in wind mode!");
                currentIsland = NextFreeIsland();
            }
            lastClicked = 0;
            for(int i= 0; i < localMap.islandLength; i++){
                // if(&localMap.islands[i] == currentIsland) continue;
                RenderWithEdges(&localMap.islands[i], GRAY);
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
            DrawText("Edit Island", WIDTH * 0.5, HEIGHT * 0.3, 20, WHITE);
            // RenderWindow(w, )
}

float distToNearestShip;
Ship *shipClicked;

Ship* NearestShip(Vector2 point){
    Ship* nearest = 0;
    float dist = 999;
    for(int i = 0 ; i < localMap.fcount; i++){
        float testDist = Vector2DistanceSqr(localMap.friendlies[i].wPos, point);
        if(testDist < dist){
            dist = testDist;
            nearest = &localMap.friendlies[i];
        }
    }
    for(int i = 0 ; i < localMap.ecount; i++){
        float testDist = Vector2DistanceSqr(localMap.enemies[i].wPos, point);
        if(testDist < dist){
            dist = testDist;
            nearest = &localMap.enemies[i];
        }
    }
    return nearest;
}


//this function is slow and way overcomplicated but it WORKS and i thought of it myself so its STAYING
void CleanUpShipArrays(Ship *array, int *setlen){
    //keep track of open spots in the array
    int realCount = 0;
    int openslots[MAX_SHIPS] = {0}; //One Plus the index of the open spot (so that we can encode zero)
    for(int i = 0 ; i < MAX_SHIPS; i++){
        if(!array[i].alive){
            //WRITE i to TAIL of openslots array (FIRST NONZERO INDEX)
            int tailOfOS = 0;
            bool reading = false; //SIGNIFIES THAT WE HAVE HIT THE DATA
            for(int o = 0; o < MAX_SHIPS; o++){
                if(openslots[o] == 0){
                    if(reading){
                        tailOfOS = o;
                        break;
                    }
                }else{
                    reading = true; //HIT REAL DATA
                }
            }
            openslots[tailOfOS] = i + 1; 

        }else{
            realCount++;
            *setlen = realCount;

            //SWAP i WITH HEAD of OPENSLOTS ARRAY
            int headOfOS = -1;
            for(int o = 0; o < MAX_SHIPS; o++){
                if(openslots[o] != 0){
                    headOfOS = o;
                    break;
                }
            }
            if(headOfOS == -1) continue;
            
            //MOVE DATA FROM ARRAY[INDEX] to ARRAY[HEAD OF OPENSLOT]
            int index = openslots[headOfOS] - 1;
            openslots[headOfOS] = 0; 
            array[index] = array[i];
            array[i] = (Ship){0};

            //WRITE i to TAIL of openslots array (FIRST NONZERO INDEX)
            int tailOfOS = 0;
            bool reading = false; //SIGNIFIES THAT WE HAVE HIT THE DATA
            for(int o = 0; o < MAX_SHIPS; o++){
                if(openslots[o] == 0){
                    if(reading){
                        tailOfOS = o;
                        break;
                    }
   
                }else{
                    reading = true; //HIT REAL DATA
                }
            }
            openslots[tailOfOS] = i + 1; 
        }
    }
}

void PlaceIslandMode(){
    DrawText("Place Islands", WIDTH * 0.5, HEIGHT * 0.3, 20, WHITE);
    currentIsland = 0;
    for(int i= 0; i < localMap.islandLength; i++){
        RenderWithEdges(&localMap.islands[i], WHITE);
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
        }
    }

    for(int i = 0; i < localMap.fcount; i++){
        if(!localMap.friendlies[i].alive)continue;
        DrawCircleV(WorldToScreen(localMap.friendlies[i].wPos), 5, BLUE);
    }
    for(int i = 0 ; i < localMap.ecount; i++){
        if(!localMap.enemies[i].alive)continue;
        DrawCircleV(WorldToScreen(localMap.enemies[i].wPos), 5, RED);
    }


    Ship *nearestShip = NearestShip(mousePos);
    if(nearestShip){
        distToNearestShip = Vector2Distance(nearestShip->wPos,mousePos);
        if(distToNearestShip < 0.1){
            DrawCircleV(WorldToScreen(nearestShip->wPos), 5, WHITE); 
            if(IsKeyPressed(KEY_BACKSPACE)){
                nearestShip->alive = false;
                if(nearestShip->team){
                    CleanUpShipArrays(localMap.friendlies, &localMap.fcount);
                }else{
                    CleanUpShipArrays(localMap.enemies, &localMap.ecount);
                }
            }
        }
    }else{
        distToNearestShip = 999;
    }

    if(IsMouseButtonPressed(0)){
        if(distToNearestShip < 0.1){
            shipClicked = nearestShip;
        }
    }
    if(IsMouseButtonDown(0)){
        if(shipClicked){
            shipClicked->wPos = mousePos;
        }
    }
    if(IsMouseButtonReleased(0)){
        indexClicked = -1;
        shipClicked = 0;
    }

    if(lastClicked){
        DrawCircleV(WorldToScreen(lastClicked->relativePosition), 20 * lastClicked->scale, BLUE);
        if(IsKeyPressed(KEY_BACKSPACE)){
            *lastClicked = (Island){0};
        }
    }

    if(localMap.fcount < MAX_SHIPS) {
        if(IsKeyPressed(KEY_K)){

            localMap.friendlies[localMap.fcount] = destroyerShip;
            localMap.friendlies[localMap.fcount].wPos = mousePos;
            localMap.fcount++;
        }
        if(IsKeyPressed(KEY_J)){
            localMap.friendlies[localMap.fcount] = BattleshipStats;
            memcpy(localMap.friendlies[localMap.fcount].batteries, BattleshipLoadout, sizeof(BattleshipLoadout)); 
            localMap.friendlies[localMap.fcount].wPos = mousePos;
            localMap.fcount++;
        }
    }else{
        DrawText("max fships allotted", 300, 400, 10, RED);
    }

    if(localMap.ecount < MAX_SHIPS) {
        if(IsKeyPressed(KEY_L)){
            localMap.enemies[localMap.ecount] = destroyerShip;
            localMap.enemies[localMap.ecount].wPos = mousePos;
            localMap.enemies[localMap.ecount].team = false;
            localMap.ecount++;
        }
            if(IsKeyPressed(KEY_SEMICOLON)){
            localMap.enemies[localMap.ecount] = BattleshipStats;
            memcpy(localMap.enemies[localMap.ecount].batteries, BattleshipLoadout, sizeof(BattleshipLoadout)); 
            localMap.enemies[localMap.ecount].wPos = mousePos;
            localMap.enemies[localMap.ecount].team = false;
            localMap.ecount++;
        }
    }else{
        DrawText("max eships allotted", 300, 430, 10, RED);
    }
}
void EditorFrameLoop(){
    ClearBackground(BLACK);
    GenericInput();

    switch (mode) {
        case WindIsland:
            EditIslandMode();
        break;
        case PlaceIsland:
           PlaceIslandMode();
        break;
    }
    DrawText(TextFormat("Islands: %d", localMap.islandLength), 400, 400, 20, BLUE);


}

void EditorUILoop(){
    DrawText("avaliable files", 30, 30, 20, BLUE);
    DrawText(TextFormat("local map: %s ", localMap.filename), 230, 30, 20, GREEN);

    bool enableHighlight = (mousePos_UIScreenCoords.x >0 && mousePos_UIScreenCoords.x < 170);

    for(int i= 0 ; i < allFiles.numStrings; i++){

        if(enableHighlight && mousePos_UIScreenCoords.y > (60 + i * 20) - 0  && mousePos_UIScreenCoords.y < (60 + i * 20) + 20){
            DrawText(StringAt(&allFiles, i), 30, 60 + i * 20, 20, WHITE);
            if(IsMouseButtonPressed(0)){
                printf("loading map: %s\n", StringAt(&allFiles, i));
                currentMap = LoadMapFile(StringAt(&allFiles, i));
                worldTime = 0;
                localMap = currentMap;
                printf("islandcount: %d\n", currentMap.islandLength);
            }
            if(IsKeyPressed(KEY_ENTER)){
                printf("rename attempt: %s \n", StringAt(&allFiles, i)); //StringAt(&allFiles, i)
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allFiles, i));
                if(rename(fullstring, "editor/newname.campaign") == 0){
                    printf("renaming!\n");
                    allFiles = GetMapNames();
                }else{
                    printf("renamefailed\n");
                }
            }
            if(IsKeyPressed(KEY_BACKSPACE)){
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allFiles, i));
                remove(fullstring);
                allFiles = GetMapNames(); 
            }
        }else{
            DrawText(StringAt(&allFiles, i), 30, 60 + i * 20, 20, BLUE);
        }
    }
    int i = allFiles.numStrings;
    if(enableHighlight && mousePos_UIScreenCoords.y > (60 + i * 20) - 0  && mousePos_UIScreenCoords.y < (60 + i * 20) + 20){
        if(IsMouseButtonPressed(0)){
            GetFile("editor/new.campaign");
            allFiles = GetMapNames();
            printf("PLUS!!!\n");
        }
        DrawText("+", 30, 60 + i * 20, 20, WHITE);
    }else{
         DrawText("+", 30, 60 + i * 20, 20, BLUE);
    }
}
   