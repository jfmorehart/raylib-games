
#pragma once
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Routine{
    const char* name;
    bool isActive;
    bool useUnscaledTime;
    float startTime;
    float duration;
    float delay;
    void (*runWhileActive)(struct Routine *routine);
} Routine;

int routineCount;
Routine routines[10];

void ExecuteRoutines();
void EndAllRoutines();

bool FindRoutineFromName(const char* routineName, Routine **routine);

bool RunRoutine(const char* routineName);