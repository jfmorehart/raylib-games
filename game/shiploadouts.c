#include "bullets.h"
#include "ships.h"
#include "shiploadouts.h"
#include "string.h"

Gun FiveInch = {
    .range = 0.2,
    .reloadTime = 5,
    .explosionRadius = 0.007,
    .spread = 0.03,
    .damage = 10,
    .shotWidth = 1
};
Gun EightInch = {
    .range = 0.3,
    .reloadTime = 4,
    .explosionRadius = 0.009,
    .spread = 0.02,
    .damage = 15,
    .shotWidth = 1
};
Gun SixteenInch = { 
    .range = 0.3,
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


Ship TransportStats = {
    .speed = 0.05,
    .batteryCount = 0,
    .alive = true,
    .health  = 80,
    .includedInScene = true,
    .scale = 0.01,
    .searchCooldown = 999,  //<= NOT THE SAME AS RELOAD TIME
    .searchRange = BATTLE_SEARCHRANGE,
};

Ship DestroyerStats = {
    .speed = 0.13,
    .batteryCount = 2,
    .alive = true,
    .health  = 150,
    .includedInScene = true,
    .scale = 0.008,
    .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
    .searchRange = BATTLE_SEARCHRANGE,
};

Ship CruiserStats = {
    .speed = 0.15,
    .batteryCount = 3,
    .alive = true,
    .health = 300,
    .includedInScene = true,
    .scale = 0.012,
    .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
    .searchRange = BATTLE_SEARCHRANGE,
};

Ship BattleshipStats = {
    .speed = 0.1,
    .batteryCount = 4,
    .alive = true,
    .health  = 600,
    .includedInScene = true,
    .scale = 0.015,
    .searchCooldown = 1,  //<= NOT THE SAME AS RELOAD TIME
    .searchRange = BATTLE_SEARCHRANGE,
};

void LoadShipIcons(){
    cruiser = LoadPolyFile("cruiser.poly");
    battleship = LoadPolyFile("battleship.poly");
    destroyer = LoadPolyFile("destroyer.poly");
    transport = LoadPolyFile("transport.poly");
}

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
    if(ship->shipName[0] == 0){
        CreateShipName_NonAlloc(ship->shipName);
    }
    if(ship->captName[0] == 0){
        CreateCaptainName_NonAlloc(ship->captName);
    }
}


// string * names;

int count_a;
int count_b;
int count_c;

char capt_a[32][32];
char capt_b[32][32];
char capt_c[32][32];


int sc_a;
int sc_b;
int sc_c;
char ship_a[32][32];
char ship_b[32][32];
char ship_c[32][32];

void LoadIntoBank(char * filename, int *count, char bank[32][32]){
    char * test = LoadFileText(filename);
    const char ** temp = TextSplit(test, '\n', count);
    for(int i = 0; i < *count; i++){
        strcpy(bank[i], temp[i]);
    }
    // UnloadFileText(filename);
}

void CreateWordBank(){

    LoadIntoBank("assets/CAPT_A.txt", &count_a, capt_a);
    LoadIntoBank("assets/CAPT_B.txt", &count_b, capt_b);
    LoadIntoBank("assets/CAPT_C.txt", &count_c, capt_c);
    

    LoadIntoBank("assets/SHIP_A.txt", &sc_a, ship_a);
    LoadIntoBank("assets/SHIP_B.txt", &sc_b, ship_b);
    LoadIntoBank("assets/SHIP_C.txt", &sc_c, ship_c);
    // printf("%d first names, %d initials, %d last names loaded.\n", count_a, count_b, count_c);

    // char capt[30];

    // snprintf(capt, 30, "%s %s %s", capt_a[rand() % count_a], capt_b[rand() % count_b], capt_c[rand() % count_c]);
    // printf("%s\n", capt);
}

void CreateCaptainName_NonAlloc(char * writeTo){
    snprintf(writeTo, 30, "%s %s %s", capt_a[rand() % count_a], capt_b[rand() % count_b], capt_c[rand() % count_c]);
}

void CreateShipName_NonAlloc(char * writeTo){
    snprintf(writeTo, 30, "%s%s %s", ship_a[rand() % sc_a], ship_b[rand() % sc_b], ship_c[rand() % sc_c]);
}

ShipLog Dehydrate(Ship * ship){
    ShipLog log;
    snprintf(log.shipName, 30, "%s", ship->shipName);
    snprintf(log.captName, 30, "%s", ship->captName);
    log.shipType = ship->shipType;
    log.health = ship->health;
    log.tfLocalOffset = ship->wPos;
    log.team = ship->team;
    return log;
}

void RehydrateShip(Ship * torehydrate, ShipLog log){
    switch (log.shipType) {
        case Destroyer:
            *torehydrate = DestroyerStats;
            memcpy(torehydrate->batteries, DestroyerLoadout, sizeof(DestroyerLoadout));
        break;
        case Battleship:
            *torehydrate = BattleshipStats;
            memcpy(torehydrate->batteries, BattleshipLoadout, sizeof(BattleshipLoadout));
        break;
        case Transport:
            *torehydrate = TransportStats;
            memset(torehydrate->batteries, 0, sizeof(DestroyerLoadout));
        break;
        case Cruiser: //not real yet
            printf("error: cruiser not implemented\n");
            // *torehydrate = CruiserStats;
            // memcpy(torehydrate->batteries, DestroyerLoadout, sizeof(CruiserLoadout));
        break;
    }
    torehydrate->shipType = log.shipType;
    snprintf(torehydrate->shipName, 30, "%s",log.shipName);
    snprintf(torehydrate->captName, 30, "%s", log.captName);
    torehydrate->health = log.health;
    torehydrate->wPos = log.tfLocalOffset; //accurate, assuming we're rehydrating at the map scene, which we should be
    torehydrate->team = log.team;
    torehydrate->alive = log.health > 1;

    InitRvecs(torehydrate);
}

