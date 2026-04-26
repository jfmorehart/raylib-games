
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "pools.h"
#include "vfx.h"

#include <math.h>
#include <stdio.h>

#define BULLET_SPEED 0.05
#define MAX_GUNS_PER_BATTERY 3

typedef struct {
    float range;
    float reloadTime;
    float explosionRadius;
    float spread;
    float damage;
} Gun;
extern Gun FiveInch;// = {0.18, 2, 0.1, 0.05};
extern Gun EightInch;// = {0.25, 4, 0.13, 0.04};
extern Gun SixteenInch;// = {0.4, 8, 0.2, 0.03};

typedef struct Ship Ship;
typedef struct Battery{
    //stats
    int gunCount;
    Gun BatteryType;
    float batterySpread;

    //hk
    float lastFireTimes[MAX_GUNS_PER_BATTERY];

    //physical
    float batteryOffset_Y;
    Vector2 batteryForward;
    float traverseAmount;

    //targeting
    Ship *shipTarget;
    float lastSearch;
    float searchCooldown;
    int timesTargeted; //for bracketing

    //for noise smoothing
    float _r_index;
}Battery;

typedef struct{
    PooledObject pObj;
    Vector2 wPos;
    Vector2 tPos;
    Vector2 right; //used for triangle
    float damage;
    float expRadius;
} Bullet;

extern int bulletCount;
extern int bulletCham;
extern Bullet bulletPool[];

typedef struct Ship Ship;

bool DamageShips(Vector2 position, float radius, Ship **enemyShips, int count, int damage);

void UpdateAndRenderBullets(Bullet *array, int bulletCount, Ship **canDamageArray, int canDamageLength);
//copy and paste of Pools Next
Bullet *NextBullet(Bullet *array, int poolSize, int *cham);

void FireBullet(Vector2 start, Vector2 target, Gun btype);

bool CanBatterySeeThis(Vector2 batteryPosition, float batteryAngle, Battery *battery, Vector2 target);

Ship *BatteryAquireTarget(const Ship *ship, Ship *targetShipsArray, int arrayLen, Battery *battery, Vector2 batteryPosition);

void BatteryEngageTarget(Vector2 batteryPosition, Battery *battery, Vector2 target);

void BatteryUpdate(const Ship *ship, Ship *targetShips, int arrayLen, Battery *battery);