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
#include "text.h"
#include <string.h>

typedef enum EditorMode{
    WindIsland,
    PlaceIsland
} EditorMode;

typedef enum EditorT{
    ShipThing,
    ObjectiveThing,
} EditorT;

typedef struct EditorThing{
    bool real; //has ever been allocated
    Vector2 position; 
    float size;
    Color color;
    EditorT type;
    void * data;
} EditorThing;

#define MAX_EDITORTHINGS 100
EditorThing thingies[MAX_EDITORTHINGS];
int editorThingCount;

extern Map mapFromDisk;
extern Vector2 worldZero;

EditorMode mode;

int currentPointCount;
Island *currentIsland;

FileType currentFileType;
Map localMap;

Vector2 dragOffset;
Island *lastClicked;
int indexClicked;

extern Ship destroyerShip;

StringArray allMaps;
StringArray allPolys;

extern TextBuffer rightBar;

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


    ClearBuffer(&rightBar);
    AddBufferText(&rightBar, "LCLICK - Select\n");
    AddBufferText(&rightBar, "ENTER - Rename\n");
    AddBufferText(&rightBar, "BKSP - delete\n");
    AddBufferText(&rightBar, "\n");
    AddBufferText(&rightBar, "Place Mode - default\n");
    AddBufferText(&rightBar, "In place mode:\n LCLICK - select\n drag - move\n BKSP - delete \n + -  rescale\n");

    AddBufferText(&rightBar, "\n");
    AddBufferText(&rightBar, "E - Edit mode\n");
    AddBufferText(&rightBar, "In edit mode:\n LCLICK - new point\n S - Save");



    worldScale = 2;
    cameraPosition = worldZero;
    mode = PlaceIsland;
    timeScale = 0;

    Map loadMap = LoadMapFile("test.situ");

    if(loadMap.islandLength > 0 && loadMap.islandLength <  ISLANDCOUNT){
        //good data
        localMap = loadMap;
    }else{
        //bad data
        localMap = (Map){0};
        AssignName(localMap.filename, "test.situ");
    }
    allMaps = GetMapNames();
    allPolys = GetPolyNames();
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
    if(IsKeyPressed(KEY_M)){
        if(localMap.objective_count < MAX_OBJECTIVES - 1){
            if(currentFileType == MapFile) {
                Objective obj = (Objective){0};
                obj.position = mousePos;
                obj.team = false;
                obj.type = Capital;
                obj.alive = true;
                localMap.map_objectives[localMap.objective_count] = obj;

                EditorThing newthing;
                newthing.data = & localMap.map_objectives[localMap.objective_count];
                newthing.type = ObjectiveThing;
                newthing.color = GOLD;
                newthing.size = 4;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++; 

                localMap.objective_count++;
            }
        }
       
    }
    if(IsKeyPressed(KEY_N)){
        if(currentFileType == MapFile) {
            if(localMap.objective_count < MAX_OBJECTIVES - 1){
                Objective obj = (Objective){0};
                obj.position = mousePos;
                obj.team = true;
                obj.alive = true;
                obj.type = Capital;
                localMap.map_objectives[localMap.objective_count] = obj;


                EditorThing newthing;
                newthing.data = & localMap.map_objectives[localMap.objective_count];
                newthing.type = ObjectiveThing;
                newthing.color = YELLOW;
                newthing.size = 4;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++; 

                localMap.objective_count++;
            }
        }
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


        //apply editor-mode changes to localMap arrays
        // localMap.objective_count = 0;
        for(int i = 0; i < editorThingCount; i++){
            if(!thingies[i].real)continue;
            if(thingies[i].type == ShipThing){
                Ship * ship = (Ship *) (thingies[i].data);
                if(!ship){
                    printf("unable to pull ship from void *\n");
                    return;
                }
                ship->wPos = thingies[i].position;
            }else if(thingies[i].type == ObjectiveThing){
                Objective * obj = (Objective *) thingies[i].data;
                if(!obj){
                    printf("unable to pull obj from void *\n");
                    return; 
                }
                obj->position = thingies[i].position;
            }
        }
     
        if(localMap.filename[0] == 0){
            printf("invalid file name when saving\n");
            return;
        }else{
            printf("beginning to save %s\n", localMap.filename);
        }

        char fullstring[30] = "editor/";
        strcat(fullstring, localMap.filename);
        
        FILE *fptr = fopen(fullstring, "wb");
        if(fptr){
            if(currentFileType == MapFile){
                // Write some text to the file
                fwrite(&localMap, sizeof(Map), 1, fptr);   
                // Close the file
                fclose(fptr); 
                printf("saved map to editor folder");
            }
            if(currentFileType == PolyPolyFile){
                PolyPoly poly = (PolyPoly){0};
                poly.islandLength = localMap.islandLength;
                memcpy(poly.islands, localMap.islands, sizeof(Island) * ISLANDCOUNT);
                memcpy(poly.filename, localMap.filename, STRINGARRAY_STRLEN);
                // Write some text to the file
                fwrite(&poly, sizeof(PolyPoly), 1, fptr);   
                // Close the file
                fclose(fptr); 
                printf("saved polyfile to editor folder");
            }

            mapFromDisk = localMap; 
            printf("named = %s\n", mapFromDisk.filename);
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

// EditorThing * storedNearestET;
EditorThing * NearestET(Vector2 point){
    EditorThing *nearest = 0;
    float dist = 999;
    for(int i = 0; i < editorThingCount; i++){
        if(!thingies[i].real) continue;
        float td = Vector2DistanceSqr(thingies[i].position, point);
        if(td < dist){
            nearest = &thingies[i];
            dist = td;
        }
    }
    return nearest;
}

float distToNearestET;
EditorThing *ETClicked;

void CleanUpETArray(EditorThing * array, int *setlen){

    //smart version for next time
    int w  = 0;
    for(int r = 0; r < MAX_EDITORTHINGS; r++){
        if(array[r].real){ //full slot
            if(!array[w].real){ //empty!
                array[w] = array[r];
                array[r] = (EditorThing){0}; 
            }
            w++;
        }
    }
    *setlen = w;
}


void LoadEditorThingsFromMap(){
    editorThingCount = 0;
    memset(thingies, 0, MAX_EDITORTHINGS * sizeof(EditorThing));

    for(int i = 0; i < localMap.fcount; i++){
        if(!localMap.friendlies[i].alive) continue;
        EditorThing newthing;
        newthing.data = &localMap.friendlies[i];
        newthing.type = ShipThing;
        newthing.color = BLUE;
        newthing.size = localMap.friendlies[i].scale;
        newthing.real = true;
        newthing.position = localMap.friendlies[i].wPos;
        thingies[editorThingCount] = newthing;
        editorThingCount++;
    }
    for(int i = 0; i < localMap.ecount; i++){
        if(!localMap.enemies[i].alive) continue;
        EditorThing newthing;
        newthing.data = &localMap.enemies[i];
        newthing.type = ShipThing;
        newthing.color = RED;
        newthing.size = localMap.enemies[i].scale;
        newthing.real = true;
        newthing.position = localMap.enemies[i].wPos;
        thingies[editorThingCount] = newthing;
        editorThingCount++;
    }
    for(int i = 0; i < localMap.objective_count; i++){
        if(!localMap.map_objectives[i].alive) continue;
        EditorThing newthing;
        newthing.data = &localMap.map_objectives[i];
        newthing.type = ObjectiveThing;
        if(localMap.map_objectives[i].team){
            newthing.color = YELLOW;
        }else{
            newthing.color = GOLD;
        }
        newthing.size = 4;
        newthing.real = true;
        newthing.position = localMap.map_objectives[i].position;
        thingies[editorThingCount] = newthing;
        editorThingCount++;
    }
}


void RebuildSubArrays(){
    int fs = 0;
    int es = 0;
    int objs = 0;

    Ship tempFs[MAX_SHIPS];
    Ship tempEs[MAX_SHIPS];
    Objective tempObs[MAX_OBJECTIVES];

    memset(tempEs, 0, MAX_SHIPS * sizeof(Ship));
    memset(tempFs, 0, MAX_SHIPS * sizeof(Ship));
    memset(tempObs, 0, MAX_OBJECTIVES * sizeof(Objective));

    for(int i = 0; i < editorThingCount; i++){
        //by this point the data should have already been cleaned. we should be in a contiguous portion of alive, real editorthing entries
        if(!thingies[i].real) {
            perror("well, shit.\n");
            continue;
        }   

        if(thingies[i].type == ShipThing){
            Ship * ship = (Ship *)thingies[i].data;
            if(!ship){
                printf("Ship data is fucked\n");
                return;
            }   
            if(ship->team){
               tempFs[fs] = *ship; //copy DATA, not location
               fs++;
            }else{
                tempEs[es] = *ship; //copy DATA, not location
                es++;
            }
        }else if(thingies[i].type == ObjectiveThing){
            Objective * obj = (Objective *)thingies[i].data;
            if(!obj){
                perror("obj data is fucked\n");
                return;
            }
            tempObs[objs] = *obj;
            objs++;
        }
    }
    memset(localMap.friendlies, 0, MAX_SHIPS * sizeof(Ship));
    memset(localMap.enemies, 0, MAX_SHIPS * sizeof(Ship));
    memset(localMap.map_objectives, 0, MAX_OBJECTIVES * sizeof(Objective));

    localMap.fcount = fs;
    localMap.ecount = es;
    localMap.objective_count = objs;
    for(int i = 0; i < localMap.fcount; i++){
        localMap.friendlies[i] = tempFs[i];
    }
    for(int i = 0; i < localMap.ecount; i++){
        localMap.enemies[i] = tempEs[i];
    }
    for(int i = 0; i < localMap.objective_count; i++){
        localMap.map_objectives[i] = tempObs[i];
    }
}

void CleanUpShipArrays(Ship *array, int *setlen){

    //smart version for next time
    // int w  = 0;
    // for(int r = 0; r < MAX_SHIPS; r++){
    //     if(array[r].alive){ //full slot
    //         if(!array[w].alive){ //empty!
    //             array[w] = array[r];
    //             array[r] = (Ship){0}; 
    //         }
    //         w++;
    //     }
    // }
    // *setlen = w;

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

    // for(int i = 0; i < localMap.fcount; i++){
    //     if(!localMap.friendlies[i].alive)continue;
    //     DrawCircleV(WorldToScreen(localMap.friendlies[i].wPos), localMap.friendlies[i].scale * 200 + 3, BLUE);
    // }
    // for(int i = 0 ; i < localMap.ecount; i++){
    //     if(!localMap.enemies[i].alive)continue;
    //     DrawCircleV(WorldToScreen(localMap.enemies[i].wPos), localMap.enemies[i].scale * 200 + 3, RED);
    // }

    // for(int i = 0 ; i < localMap.objective_count; i++){
    //     if(localMap.map_objectives[i].team){
    //         DrawCircleV( WorldToScreen(localMap.map_objectives[i].position), 8, YELLOW);    
    //     }else{
    //         DrawCircleV(WorldToScreen(localMap.map_objectives[i].position), 8, GOLD);
    //     }
    // }

    // printf("editor thing count:%d\n", editorThingCount);

    for(int i = 0; i < MAX_EDITORTHINGS; i++){
        if(!thingies[i].real) continue;
        DrawCircleV(WorldToScreen(thingies[i].position), 5, thingies[i].color);
    }


    EditorThing * nearestET = NearestET(mousePos);
    if(nearestET){
        distToNearestET = Vector2Distance(nearestET->position,mousePos);
        if(distToNearestET < 0.1){
            DrawCircleV(WorldToScreen(nearestET->position), 5, GRAY); 
            if(IsKeyPressed(KEY_BACKSPACE)){
                nearestET->real= false;
                if(nearestET->type == ShipThing){

                    //todo unfuck
                    Ship * ship = (Ship *)nearestET->data;
                    if(!ship){
                        printf("Ship data is fucked\n");
                        return;
                    }   
                    ship->alive = false;
                }
                CleanUpETArray(thingies, &editorThingCount);
                RebuildSubArrays(); //this breaks all the ET array pointers
                LoadEditorThingsFromMap(); //this fixes them
            }
        }
    }else{
        distToNearestET = 999;
    }

    if(IsMouseButtonPressed(0)){
        if(distToNearestET < 0.1){
            ETClicked = nearestET;
        }
    }
    if(IsMouseButtonDown(0)){
        if(ETClicked){
            ETClicked->position = mousePos;
        }
    }
    if(IsMouseButtonReleased(0)){
        indexClicked = -1;
        ETClicked = 0;
    }

    if(lastClicked){
        DrawCircleV(WorldToScreen(lastClicked->relativePosition), 20 * lastClicked->scale, BLUE);
        if(IsKeyPressed(KEY_BACKSPACE)){
            *lastClicked = (Island){0};
        }
    }
    // printf("ecount = %d\n", localMap.ecount);
    if(currentFileType == MapFile){
            
        if(localMap.fcount < MAX_SHIPS) {
            if(IsKeyPressed(KEY_K)){

                localMap.friendlies[localMap.fcount] = destroyerShip;
                localMap.friendlies[localMap.fcount].wPos = mousePos;
                localMap.friendlies[localMap.fcount].team = true;

                EditorThing newthing;
                newthing.data = &localMap.friendlies[localMap.fcount];
                newthing.type = ShipThing;
                newthing.color = BLUE;
                newthing.size = 3;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++;
                localMap.fcount++;
               printf("fcount++\n");
                printf("new friendly destroyer\n");

            }
            if(IsKeyPressed(KEY_J)){
                localMap.friendlies[localMap.fcount] = BattleshipStats;
                memcpy(localMap.friendlies[localMap.fcount].batteries, BattleshipLoadout, sizeof(BattleshipLoadout)); 
                localMap.friendlies[localMap.fcount].wPos = mousePos;
                localMap.friendlies[localMap.fcount].team = true;
                localMap.friendlies[localMap.fcount].alive = true;
 

                EditorThing newthing;
                newthing.data = &localMap.friendlies[localMap.fcount];
                newthing.type = ShipThing;
                newthing.color = BLUE;
                newthing.size = 5;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++;
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


                EditorThing newthing;
                newthing.data = &localMap.enemies[localMap.ecount];
                newthing.type = ShipThing;
                newthing.color = RED;
                newthing.size = 3;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++;
                localMap.ecount++;
            }
            if(IsKeyPressed(KEY_SEMICOLON)){
                localMap.enemies[localMap.ecount] = BattleshipStats;
                memcpy(localMap.enemies[localMap.ecount].batteries, BattleshipLoadout, sizeof(BattleshipLoadout)); 
                localMap.enemies[localMap.ecount].wPos = mousePos;
                localMap.enemies[localMap.ecount].team = false;



                EditorThing newthing;
                newthing.data = &localMap.enemies[localMap.ecount];
                newthing.type = ShipThing;
                newthing.color = RED;
                newthing.size = 5;
                newthing.real = true;
                newthing.position = mousePos;
                thingies[editorThingCount] = newthing;
                editorThingCount++;
                localMap.ecount++;
            }
        }else{
            DrawText("max eships allotted", 300, 430, 10, RED);
        }
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

    float diff = (WIDTH - HEIGHT) * 0.4;
    int border = 30;
    DrawText(rightBar.charArray, WIDTH * RSCALE - border - diff - 25, 35, 18, GRAY);

    DrawText("avaliable campaigns", 30, 30, 20, BLUE);
    DrawText(TextFormat("local map: %s ", localMap.filename), 230, 30, 20, GREEN);

    bool enableHighlight = (mousePos_UIScreenCoords.x >0 && mousePos_UIScreenCoords.x < 170);

    for(int i= 0 ; i < allMaps.numStrings; i++){

        if(enableHighlight && mousePos_UIScreenCoords.y > (60 + i * 20) - 0  && mousePos_UIScreenCoords.y < (60 + i * 20) + 20){
            DrawText(StringAt(&allMaps, i), 30, 60 + i * 20, 20, WHITE);
            if(IsMouseButtonPressed(0)){
                printf("loading map: %s\n", StringAt(&allMaps, i));
                mapFromDisk = LoadMapFile(StringAt(&allMaps, i));
                worldTime = 0;
                localMap = mapFromDisk;
                editorThingCount = 0;
                memset(thingies, 0, sizeof(EditorThing) * MAX_EDITORTHINGS);
                LoadEditorThingsFromMap();
                currentFileType = MapFile;
                printf("islandcount: %d\n", mapFromDisk.islandLength);
            }
            if(IsKeyPressed(KEY_ENTER)){
                printf("rename attempt: %s \n", StringAt(&allMaps, i)); //StringAt(&allFiles, i)
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allMaps, i));
                if(rename(fullstring, "editor/newname.campaign") == 0){
                    printf("renaming!\n");
                    allMaps = GetMapNames();
                }else{
                    printf("renamefailed\n");
                }
            }
            if(IsKeyPressed(KEY_BACKSPACE)){
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allMaps, i));
                remove(fullstring);
                allMaps = GetMapNames(); 
            }
        }else{
            DrawText(StringAt(&allMaps, i), 30, 60 + i * 20, 20, BLUE);
        }
    }
    int i = allMaps.numStrings;
    if(enableHighlight && mousePos_UIScreenCoords.y > (60 + i * 20) - 0  && mousePos_UIScreenCoords.y < (60 + i * 20) + 20){
        if(IsMouseButtonPressed(0)){
            // GetOrMakeFile("editor/new.campaign");
            if(!FileCheck("editor/new.campaign")){
                FILE * fptr = fopen(("editor/new.campaign"), "w");
                fwrite(&(Map){0}, sizeof(Map), 1, fptr);   
                fclose(fptr);
            }
            allMaps = GetMapNames();
            printf("PLUS!!!\n");
        }
        DrawText("+", 30, 60 + i * 20, 20, WHITE);
        
    }else{
        DrawText("+", 30, 60 + i * 20, 20, BLUE);
    }

    int rownumber = allMaps.numStrings + 5;
    DrawText("avaliable images", 30, 60 + (rownumber) * 20, 20, BLUE);
    rownumber++;

    for(int i = 0; i < allPolys.numStrings; i++){
        if(enableHighlight && mousePos_UIScreenCoords.y > (60 + (i + rownumber) * 20) - 0  && mousePos_UIScreenCoords.y < (60 + (i + rownumber) * 20) + 20){
            DrawText(StringAt(&allPolys, i), 30, 60 + (i + rownumber) * 20, 20, WHITE); 
            if(IsMouseButtonPressed(0)){
                printf("loading poly: %s\n", StringAt(&allPolys, i));

                //clean point list
                editorThingCount = 0;
                memset(thingies, 0, sizeof(EditorThing) * MAX_EDITORTHINGS);

                PolyPoly poly = LoadPolyFile(StringAt(&allPolys, i));
                printf("poly island count = %d\n", poly.islandLength);
                mapFromDisk = (Map){0};
                memcpy(mapFromDisk.filename, poly.filename, STRINGARRAY_STRLEN);
                mapFromDisk.islandLength = poly.islandLength;
                memcpy(mapFromDisk.islands, poly.islands, sizeof(Island) * ISLANDCOUNT);
                currentFileType = PolyPolyFile;
                worldTime = 0;
                localMap = mapFromDisk;
                printf("islandcount: %d\n", mapFromDisk.islandLength);
            }
            if(IsKeyPressed(KEY_ENTER)){
                printf("rename attempt: %s \n", StringAt(&allPolys, i)); //StringAt(&allFiles, i)
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allPolys, i));
                if(rename(fullstring, "editor/newname.poly") == 0){
                    printf("renaming!\n");
                    allPolys = GetPolyNames();
                }else{
                    printf("renamefailed\n");
                }
            }
            if(IsKeyPressed(KEY_BACKSPACE)){
                char fullstring[30] = "editor/";
                strcat(fullstring, StringAt(&allPolys, i));
                remove(fullstring);
                allPolys = GetPolyNames(); 
            }
        }else{
            DrawText(StringAt(&allPolys, i), 30, 60 + (i + rownumber) * 20, 20, BLUE);  
        }
    }

    rownumber = allPolys.numStrings + allMaps.numStrings + 6;

    //render plus sign
    if(mousePos_UIScreenCoords.y > (60 + (rownumber + 1) * 20) - 0  && mousePos_UIScreenCoords.y < (60 + (rownumber+ 1) * 20) + 20){
        DrawText("+", 30, 60 + (rownumber + 1) * 20, 20, WHITE);
        if(IsMouseButtonPressed(0)){
            // GetOrMakeFile("editor/new.campaign");
            if(!FileCheck("editor/new.poly")){
                fopen(("editor/new.poly"), "w");
            }
            allPolys = GetPolyNames();
        }

    }else{
        DrawText("+", 30, 60 + (rownumber + 1) * 20, 20, BLUE);
    }
}
   