    #pragma once
    #include "bullets.h"
    #include "ships.h"

    Battery DestroyerLoadout[SHIP_MAXBATTERIES];

    extern Gun FiveInch;

    Vector2 shipForward = (Vector2){1, 0};
    Vector2 shipBack= (Vector2){-1, 0};
    Vector2 shipLeft= (Vector2){0, -1};
    Vector2 shipRight = (Vector2){0, 1};

    Battery threeGun = {
        .gunCount = 3, // <= MAX GUNS PER BATTERY 
        .batterySpread = 0.2,
        .lastFireTimes = {0, 0, 0},  //<= MAX GUNS PER BATTERY 
        .batteryOffset_Y = 1,
        .batteryForward = (Vector2){0, 0},
        .traverseAmount = 90,
        .searchCooldown = 1, 
    };
        Battery twoGun = {
        .gunCount = 2, // <= MAX GUNS PER BATTERY 
        .batterySpread = 0.2,
        .lastFireTimes = {0, 0},  //<= MAX GUNS PER BATTERY 
        .batteryOffset_Y = 1,
        .batteryForward = (Vector2){0, -1},
        .traverseAmount = 190,
        .searchCooldown = 1, 
    };

    Ship DestroyerStats = {
        .alive = true,
        .health  = 100,
        .includedInScene = true,
        .scale = 0.01,
        .searchCooldown = 1,
        .searchRange = SHIP_SEARCHRANGE * SHIP_SEARCHRANGE,
    };


    void MakeLoadouts(){
        DestroyerLoadout[0] = twoGun;
        DestroyerLoadout[0].BatteryType = FiveInch;
        DestroyerLoadout[0].batteryForward = shipForward;
        DestroyerLoadout[0].batteryOffset_Y = 1;

        DestroyerLoadout[1] = twoGun;
        DestroyerLoadout[1].BatteryType = FiveInch;
        DestroyerLoadout[1].batteryForward = shipBack;
        DestroyerLoadout[1].batteryOffset_Y = -1;

        DestroyerLoadout[2] = twoGun;
        DestroyerLoadout[2].BatteryType = FiveInch;
        DestroyerLoadout[2].BatteryType.range *= 1.2;
        DestroyerLoadout[2].batteryForward = shipForward;
        DestroyerLoadout[2].batteryOffset_Y = -1;
        DestroyerLoadout[2].traverseAmount = 70;
    }


