
#include <stdio.h>
#include "taskforce.h"
#include "map.h"
#include "mapscene.h"
#include "ships.h"
#include "shiploadouts.h"

int taskForceCount = 0;
TaskForce activeTFs[MAX_TFS];


void DehydrateTaskForces(int tf_count, TaskForce * tfs, Fleet * friendly, Fleet * enemy){
    *friendly = (Fleet){0};
    friendly->team = true;

    *enemy = (Fleet){0};
    enemy->team = false;

    for(int i = 0; i < tf_count; i++){
        DryTF * dry = 0;
        if(tfs[i].team){
            dry = &friendly->tfs[friendly->tf_count];
            friendly->tf_count++;
        }else{
            dry = &enemy->tfs[enemy->tf_count];
            enemy->tf_count++;
        }
        snprintf(dry->name, 20, "%s", tfs[i].name);
        dry->position = tfs[i].position;
        dry->destination = tfs[i].destination;

        int liveWrites = 0;
        for(int s = 0; s < tfs[i].shipCount; s++){
            if(tfs[i].ships[s]->alive){
                dry->logs[liveWrites] = Dehydrate(tfs[i].ships[s]);
                dry->logs[liveWrites].tfLocalOffset = Vector2Subtract(tfs[i].ships[s]->wPos, tfs[i].position);
                liveWrites++;
            }
        }
        dry->shipCount = liveWrites;
    }
}
void RehydrateTaskForces(int * tf_length, TaskForce *torehydrate, Fleet * friendly, Fleet * enemy, Map * placeShipsIn){
    for(int i = 0; i < friendly->tf_count; i++){
        TaskForce * tr = &torehydrate[*tf_length];
        *tr = (TaskForce){0};
        DryTF from = friendly->tfs[i];

        *tf_length += 1;

        tr->team = true;
        snprintf(tr->name, 20, "%s", from.name);
        tr->destination = from.destination;
        tr->position = from.position;
        tr->shipCount = from.shipCount;

        tr->min_speed = 0.09;
        tr->selected = false;

        for(int s = 0; s < from.shipCount; s++){
            Ship * slot = &placeShipsIn->friendlies[placeShipsIn->fcount];
            RehydrateShip(slot, from.logs[s]);
            slot->wPos = Vector2Add(slot->wPos, tr->position);
            tr->ships[s] = slot;
            placeShipsIn->fcount++;
        }
    }
    for(int i = 0; i < enemy->tf_count; i++){
        TaskForce * tr = &torehydrate[*tf_length];
        *tr = (TaskForce){0};
        DryTF from = enemy->tfs[i];

        *tf_length += 1;

        tr->team = false;
        snprintf(tr->name, 20, "%s", from.name);
        tr->destination = from.destination;
        tr->position = from.position;
        tr->shipCount = from.shipCount;

        tr->min_speed = 0.09;
        tr->selected = false;

        for(int s = 0; s < from.shipCount; s++){
            Ship * slot = &placeShipsIn->enemies[placeShipsIn->ecount];
            RehydrateShip(slot, from.logs[s]);
            slot->wPos = Vector2Add(slot->wPos,tr->position);
            tr->ships[s] = slot;
            placeShipsIn->ecount++;
        }
    }
}
void RehydrateTFsFromDisk(Map * toreh){
    //load active fleet info
    char filepath [30];

    ReAppendSuffix(filepath, toreh->filename, "_f.fleet");
    Fleet friendlyFleet = LoadFleetFile(filepath);
    ReAppendSuffix(filepath, toreh->filename, "_e.fleet");
    Fleet enemyFleet = LoadFleetFile(filepath);
    taskForceCount = 0;
    RehydrateTaskForces(&taskForceCount, activeTFs, &friendlyFleet, &enemyFleet, toreh);
    // RehydrateTaskForces(int *tf_length, TaskForce *torehydrate, Fleet *friendly, Fleet *enemy, Map *placeShipsIn)
    //fallback to random/test fleet info
}

void TFShipsLocalToWorld(){
    for(int t = 0; t < taskForceCount; t++){ 
        //wPos stores offset from tf center in transit
        for(int s= 0; s < activeTFs[t].shipCount; s++){
            activeTFs[t].ships[s]->wPos = Vector2Add(activeTFs[t].ships[s]->wPos, activeTFs[t].position);
        }
    }
}
void TFShipsWorldToLocal(){
    for(int t = 0; t < taskForceCount; t++){ 
        for(int s= 0; s < activeTFs[t].shipCount; s++){
            //wPos stores offset from tf center in transit
            activeTFs[t].ships[s]->wPos = Vector2Subtract(activeTFs[t].ships[s]->wPos, activeTFs[t].position);
        }
    }
}
void CreateTaskForcesFromMapFile(Map *map){

    taskForceCount = 0; 
    for(int i =0 ; i < map->fcount; i++){

        bool found = false;
        if(!map->friendlies[i].alive)continue;
        InitRvecs(&map->friendlies[i]);

        for(int t = 0; t < taskForceCount; t++){ 
            if(activeTFs[t].shipCount + 1 >= MAX_SHIPS_IN_TF) continue;
            if(activeTFs[t].team != map->friendlies[i].team) continue;
            if(Vector2Distance(map->friendlies[i].wPos, activeTFs[t].position) < TF_MAX_RADIUS){
                activeTFs[t].ships[activeTFs[t].shipCount] = &map->friendlies[i];
                activeTFs[t].shipCount++;
                found = true;
                break;
            }   
        }
        if(found) continue;
        //TRIED ALL TFS AND DIDNT FIT INTO ANY!
        //make new!

        activeTFs[taskForceCount] = (TaskForce){0};
        snprintf(activeTFs[taskForceCount].name, 20, "TF %d", (taskForceCount + 2) * 21);
        // tfs[taskForceCount].name = "Task Force" + taskForceCount.toString();

        activeTFs[taskForceCount].shipCount = 0;
        activeTFs[taskForceCount].min_speed = 0.09;
        activeTFs[taskForceCount].team = map->friendlies[i].team;
        activeTFs[taskForceCount].position = map->friendlies[i].wPos;
        activeTFs[taskForceCount].ships[activeTFs[taskForceCount].shipCount] = &map->friendlies[i];
        // activeTFs[taskForceCount].ships[activeTFs[taskForceCount].shipCount]->wPos = Vector2Zero(); //center the first one
        activeTFs[taskForceCount].shipCount++;
        taskForceCount++;

        // printf("spawning new taskforce- ship - %f, %f", currentMap.friendlies[i].wPos.x,  tfs[taskForceCount].position.x);
    }

    for(int i =0 ; i < map->ecount; i++){

        bool found = false;
        if(!map->enemies[i].alive)continue;
        InitRvecs(&map->enemies[i]);
        for(int t = 0; t < taskForceCount; t++){ 
            if(activeTFs[t].shipCount + 1 >= MAX_SHIPS_IN_TF) continue;
            if(activeTFs[t].team != map->enemies[i].team) continue;
            if(Vector2Distance(map->enemies[i].wPos, activeTFs[t].position) < TF_MAX_RADIUS){
                activeTFs[t].ships[activeTFs[t].shipCount] = &map->enemies[i];
                activeTFs[t].shipCount++;
                found = true;
                break;
            }   
        }
        if(found) continue;
        //TRIED ALL TFS AND DIDNT FIT INTO ANY!
        //make new!

        activeTFs[taskForceCount].shipCount = 0;
        activeTFs[taskForceCount] = (TaskForce){0};
        activeTFs[taskForceCount].min_speed = 0.09;
        activeTFs[taskForceCount].team = false;
        activeTFs[taskForceCount].position = map->enemies[i].wPos;
        activeTFs[taskForceCount].ships[activeTFs[taskForceCount].shipCount] = &map->enemies[i];
        // activeTFs[taskForceCount].ships[activeTFs[taskForceCount].shipCount]->wPos = Vector2Zero(); // center the first one
        activeTFs[taskForceCount].shipCount++;
        activeTFs[taskForceCount].destination = PickRandomLegalDestination(activeTFs[taskForceCount].position);
        
        taskForceCount++;
    }

}
