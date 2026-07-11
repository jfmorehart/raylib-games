
#pragma once

#include "bullets.h"
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "globals.h"
#include "islands.h"

#include <stdio.h>

#define SHIPTURN 2
#define SHIPSPEED 0.1
#define SHIP_BLEND_MAX 0.15
#define SHIP_SEARCHRANGE 0.2
#define SHIP_MAXBATTERIES 6

typedef struct Ship{
    //basics
    bool alive;
    bool includedInScene;
    bool isVisible;// always true 

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
    
    float illuminationThisFrame;

    Battery batteries[SHIP_MAXBATTERIES];
} Ship;


bool IsPointInShip(Vector2 worldSpace, const Ship *ship, float scaleMult);

float Path2Target(const Ship *ship, int rays, float fanAngle, Vector2 target, Island *obstacles);

void RenderShip(const Ship *ship, float scaleMult);
void RenderShipColor(const Ship *ship, float scaleMult, Vector3 color);

void SteerShip(Ship *ship, bool avoidIslands, Island *islandsToAvoid);
void SteerShipBattle(Ship *ship, bool avoidIslands, Island *islandsToAvoid);

void BattleSceneIntroReset(Ship *ship);
void ShipCombat(Ship *ship, Ship *targetShipsArray, int arrayLen);
