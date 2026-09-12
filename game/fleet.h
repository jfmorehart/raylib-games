
#pragma once
#include "ships.h"

typedef struct DryTF{
    char name[20];
    Vector2 position;
    int shipCount;
    Vector2 destination;
    ShipLog logs[MAX_SHIPS_IN_TF];
}DryTF;

typedef struct Fleet{
    bool team;
    int tf_count;
    DryTF tfs[MAX_TFS];
}Fleet;
