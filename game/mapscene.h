#pragma once
#include "routines.h"

#define DAY_LENGTH 2.5
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