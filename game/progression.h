
#pragma once
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "ships.h"
#include "fleet.h"
#include "map.h"

typedef enum Admirals{
    Raenin, 
    Lutzo, 
    Kenning, 
    Artem
}Admirals;
Admirals selected;

typedef struct Campaign{
    char filepath [30];
    Admirals name;
    MapRecord maps [5];
    Fleet starterFleet;
    Fleet enemyStarterFleet;
}Campaign;

typedef struct SaveFile {
    Campaign campaign;
    Fleet activeFleet;
    Fleet activeEnemyFleet;
    int stage; //2, 1, 0, -1, -2
} SaveFile;

void CreateCampaigns();
void OnCompleteMap();

