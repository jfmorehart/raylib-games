

#include <stdlib.h>
#include "raylib.h"
#include "routines.h"


int routineCount;
Routine routines[10];

void ExecuteRoutines(){
    for(int i = 0; i < routineCount; i++){
        if(routines[i].isActive){
            routines[i].runWhileActive(&routines[i]);
        }
    }
}
void EndAllRoutines(){
    for(int i = 0; i < routineCount; i++){
        routines[i].isActive = false;
    }
}

bool FindRoutineFromName(const char* routineName, Routine **routine){
    for(int i = 0; i < routineCount; i++){
        if(strcmp(routines[i].name,routineName) == 0){
            *routine = &routines[i];
            return true;
        }
    }
    return false;
}

bool RunRoutine(const char* routineName){
    Routine *routine = NULL;
    if(FindRoutineFromName(routineName, &routine)){
        //todo fix time
        // printf("found smth");
        if(unscaledTime - routine->startTime < routine->delay) return false;
        routine->startTime = unscaledTime;
        routine->isActive = true;
        return true;
    }else{
        TraceLog(LOG_FATAL, "Couldnt find routine %s\n", routineName);
    }
    return false;
}