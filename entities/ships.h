
#pragma once

#include "bullets.h"
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"

#include <stdio.h>

#define SHIPTURN 2
#define SHIPSPEED 1
#define SHIP_BLEND_MAX 0.15
#define SHIP_SEARCHRANGE 0.35
#define SHIP_MAXBATTERIES 5

typedef struct Ship{
    //basics
    bool alive;
    bool includedInScene;

    bool team;
    Vector2 wPos;
    float angle;
    float scale;
    float health;

    //movement
    bool hasMoveTarget;
    Vector2 moveTargetPosition;
    bool selected;

    //combat
    float searchCooldown;
    float lastSearch;
    float searchRange;
    struct Ship *targetShip;

    int batteryCount;
    Battery batteries[SHIP_MAXBATTERIES];
} Ship;

extern Island island[ISLANDCOUNT];

float Path2Target(const Ship *ship, int rays, float fanAngle, Vector2 target);

void RenderShip(const Ship *ship, float scaleMult);

void SteerShip(Ship *ship, float speedMult);

void ShipCombat(Ship *ship, Ship *targetShipsArray, int arrayLen);
