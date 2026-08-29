
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "ships.h"

#define MAX_SHIPS_IN_TF 10
#define TF_MAX_RADIUS 0.5

typedef struct TaskForce {
    bool team;
    char name[20];
    Vector2 position;
    float min_speed;
    float max_detection_range;
    int shipCount;
    Ship * ships[MAX_SHIPS_IN_TF];
    Vector2 destination;
    bool selected;
} TaskForce;

#define MAX_TFS 50
int taskForceCount = 0;
TaskForce tfs[MAX_TFS];


