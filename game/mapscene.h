#pragma once
#include "routines.h"
#include "taskforce.h"

#define DAY_LENGTH 5
#define MAP_SEARCHRANGE 0.3

void TimeRoutine(Routine *routine);


void SwitchToBattleRoutine(Routine * routine);

void FocusRoutine(Routine *routine);

void RandomizeMap();
void InitMapScene();
void MapInputLoop();
void MapFrameLoop();
void MapUIRender();
void LostBattleSwitch();
void WonBattleSwitch();
void DisengageBattleSwitch();

void DehydrateTaskForces(int tf_count, TaskForce * tfs, Fleet * friendly, Fleet * enemy);
void RehydrateTaskForces(int * tf_length, TaskForce *torehydrate, Fleet * friendly, Fleet * enemy, Map * placeShipsIn);