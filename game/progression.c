
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "ships.h"
#include "fleet.h"
#include "map.h"
#include "progression.h"
#include "filesystem.h"
#include "taskforce.h"


int currentStage;

Campaign activeCampaign;
Fleet activeFleet;
Fleet activeEnemyFleet;
extern TaskForce activeTFs[MAX_TFS];
extern int taskForceCount;

extern Map mapFromDisk;

void OnCompleteMap(){

    //save activeFleet
    TFShipsLocalToWorld();
    DehydrateTaskForces(taskForceCount, activeTFs, &activeFleet, &activeEnemyFleet);

    //load next map
    currentStage++;

    RehydrateMap(&mapFromDisk, &activeCampaign.maps[currentStage]);
    taskForceCount = 0;
    RehydrateTaskForces(&taskForceCount, activeTFs, &activeFleet, &activeEnemyFleet, &mapFromDisk);

    //load cutscene 4 next map
    SwitchScenes(TroopScene);
}

void CreateCampaigns(){

    Campaign raenin = (Campaign){0};

    AssignName(raenin.filepath, "editor/raenin.campaign");
    raenin.name = Raenin;
    raenin.enemyStarterFleet = LoadFleetFile("raenin0_e.fleet");
    raenin.starterFleet = LoadFleetFile("raenin0_f.fleet");
    raenin.maps[0] = LoadMapRecord("raenin0.map");
    raenin.maps[1] = LoadMapRecord("raenin1.map");
    raenin.maps[1] = LoadMapRecord("raenin2.map");


    activeCampaign = raenin;
    currentStage = 0;
}