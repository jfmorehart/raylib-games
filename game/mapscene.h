#pragma once
#include "routines.h"

void TimeRoutine(Routine *routine);


void SwitchToBattleRoutine(Routine * routine);

void FocusRoutine(Routine *routine);

void RandomizeMap();
void InitMapScene();
void MapInputLoop();
void MapFrameLoop();