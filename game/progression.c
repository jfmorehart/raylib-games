
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

    strcpy(raenin.enemyActionText[0], "Multiple intelligence reports point towards a convoy leaving Xohtec tonight.\n It will arrive in Lilben in four days.\n\nIt is imperative to the party that it is not allowed to reach its destination.\n");
    strcpy(raenin.missionText[0], "Admiral Raenin, you have been tasked with intercepting \nthe Luhansk convoy at ANY COST. Failure to do this will \nresult in a naval court martial and demotion.\n\nYour loyalty is in question.\n");

    strcpy(raenin.enemyActionText[1], "test 002 ");
    strcpy(raenin.missionText[1], "mission 002 ");

    strcpy(raenin.enemyActionText[2], "test 003 ");
    strcpy(raenin.missionText[2], "mission 003 ");

    activeCampaign = raenin;
    currentStage = 0;
}

char * GetEnemyActionText(){
    return activeCampaign.enemyActionText[currentStage];
}
char * GetMissionText(){
    return activeCampaign.missionText[currentStage];
}