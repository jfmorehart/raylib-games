    #include "bullets.h"
    #include "ships.h"
    #include "shiploadouts.h"



    Gun FiveInch = {
        .range = 0.3,
        .reloadTime = 5,
        .explosionRadius = 0.007,
        .spread = 0.03,
        .damage = 10,
        .shotWidth = 1
    };
    Gun EightInch = {
        .range = 0.5,
        .reloadTime = 4,
        .explosionRadius = 0.009,
        .spread = 0.02,
        .damage = 15,
        .shotWidth = 1
    };
    Gun SixteenInch = { 
        .range = 0.8,
        .reloadTime = 8,
        .explosionRadius = 0.012,
        .spread = 0.01,
        .damage = 30,
        .shotWidth = 3
    };
    Battery DestroyerLoadout[SHIP_MAXBATTERIES];
    Battery BattleshipLoadout[SHIP_MAXBATTERIES];

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
        .traverseAmount = 180,
        .searchCooldown = 1,   //<= NOT THE SAME AS RELOAD TIME
    };
        Battery twoGun = {
        .gunCount = 2, // <= MAX GUNS PER BATTERY 
        .batterySpread = 0.2,
        .lastFireTimes = {0, 0},  //<= MAX GUNS PER BATTERY 
        .batteryOffset_Y = 1,
        .batteryForward = (Vector2){0, -1},
        .traverseAmount = 250,
        .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
    };

    Ship DestroyerStats = {
        .batteryCount = 2,
        .alive = true,
        .health  = 100,
        .includedInScene = true,
        .scale = 0.007,
        .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
        .searchRange = SHIP_SEARCHRANGE * SHIP_SEARCHRANGE,
    };

    Ship BattleshipStats = {
        .batteryCount = 4,
        .alive = true,
        .health  = 600,
        .includedInScene = true,
        .scale = 0.0125,
        .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
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

        BattleshipLoadout[0] = threeGun;
        BattleshipLoadout[0].BatteryType = SixteenInch;
        BattleshipLoadout[0].batteryForward = shipForward;
        BattleshipLoadout[0].batteryOffset_Y = 1;

        BattleshipLoadout[1] = threeGun;
        BattleshipLoadout[1].BatteryType = SixteenInch;
        BattleshipLoadout[1].batteryForward = shipBack;
        BattleshipLoadout[1].batteryOffset_Y = -1;

        BattleshipLoadout[2] = threeGun;
        BattleshipLoadout[2].BatteryType = SixteenInch;
        BattleshipLoadout[2].batteryForward = shipRight;
        BattleshipLoadout[2].batteryOffset_Y = 0;

        BattleshipLoadout[3] = threeGun;
        BattleshipLoadout[3].BatteryType = SixteenInch;
        BattleshipLoadout[3].batteryForward = shipLeft;
        BattleshipLoadout[3].batteryOffset_Y = 0;
    }

    void InitRvecs(Ship *ship){
        for(int i = 0; i < ship->batteryCount; i++){
            ship->batteries[i]._r_index = R01();
        }
    }


