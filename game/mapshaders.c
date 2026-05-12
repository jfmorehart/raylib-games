// #include "raylib.h"
// #include "raymath.h"
// #include <math.h>  
#include "mapshaders.h"     
#include <stdlib.h>
#include "globals.h"

DotShader CreateDotShader(Shader shader){
    DotShader ds = {0};
    ds.shader = shader;
    ds.tloc = GetShaderLocation(ds.shader, "_Time");
    ds.resLoc =  GetShaderLocation(ds.shader, "resolution"); 
    ds.multLoc = GetShaderLocation(ds.shader, "multiplier");                                                                                                                                   
    ds.mposLoc = GetShaderLocation(ds.shader, "mpos");   
    ds.dloc = GetShaderLocation(ds.shader, "dotsize");    
    ds.wsLoc = GetShaderLocation(ds.shader, "worldScale");   
    ds.camLoc = GetShaderLocation(ds.shader, "cameraPosition");    
    ds.colLoc = GetShaderLocation(ds.shader, "dotcolor"); 
    return ds;
}
void SetRes(DotShader *ds, Vector2 res){
    SetShaderValue(ds->shader, ds->resLoc, &res, SHADER_UNIFORM_VEC2); 
}

void FrameRefreshShader(DotShader *ds, float time, Vector2 camPos, float worldScale, Vector2 special_Mpos){
    SetShaderValue(ds->shader, ds->tloc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->wsLoc, &worldScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->camLoc, &camPos, SHADER_UNIFORM_VEC2); 
    SetShaderValue(ds->shader, ds->mposLoc, &special_Mpos, SHADER_UNIFORM_VEC2); 
}

void DotShaderValues(DotShader *ds, float dotSize, int multiplier, Vector3 color){
    SetShaderValue(ds->shader, ds->dloc, &dotSize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->multLoc, &multiplier, SHADER_UNIFORM_INT);
    SetShaderValue(ds->shader, ds->colLoc, &color, SHADER_UNIFORM_VEC3);
}

DotShader islandShader;
DotShader oceanShader;
DotShader generalShader;
DotShader explosionShader;
DotShader lightShader;
DotShader illuminatedShader;
DotShader skyShader;
DotShader waterShader;

extern SceneName currentScene;

int ShaderInit(){


    // int es_colLoc = GetShaderLocation(explosionShader, "color");
    // SetShaderValue(explosionShader, es_colLoc, &col, SHADER_UNIFORM_VEC3);

    islandShader = CreateDotShader(LoadShader(0, "shaders/island.fs"));
    oceanShader = CreateDotShader(LoadShader(0, "shaders/ocean.fs"));
    generalShader = CreateDotShader(LoadShader(0, "shaders/generaldot.fs"));
    explosionShader= CreateDotShader(LoadShader(0, "shaders/explosion.fs"));
    lightShader = CreateDotShader(LoadShader("shaders/beam.vs","shaders/beam.fs"));
    illuminatedShader = CreateDotShader(LoadShader(0,"shaders/illum.fs")); 
    skyShader = CreateDotShader(LoadShader(0,"shaders/sky.fs")); 
    waterShader = CreateDotShader(LoadShader(0,"shaders/water.fs")); 

    DotShaderValues(&generalShader, 0.2, 50, (Vector3){1, 1, 1});
    DotShaderValues(&explosionShader, 0.3, 230, (Vector3){1, 1, 1});
    DotShaderValues(&lightShader, 0.3, 230, (Vector3){0.2, 0.2, 0.2});

    Vector2 resolutionVector = {WIDTH, HEIGHT};  
    SetRes(&islandShader, resolutionVector);
    SetRes(&oceanShader, resolutionVector);
    SetRes(&generalShader, resolutionVector);
    SetRes(&explosionShader, resolutionVector); 
    SetRes(&lightShader, resolutionVector); 
    SetRes(&illuminatedShader, resolutionVector); 
    SetRes(&skyShader, resolutionVector); 
    SetRes(&waterShader, resolutionVector); 
    return 0;
}

void PrepOceanPass(Vector2 specialMousePos, int multiplier, float dotsize){

    //setup OCEAN CONSTANTS
    DotShaderValues(&oceanShader, dotsize, multiplier, (Vector3){0 ,0, 0});
    FrameRefreshShader(&oceanShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&islandShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&explosionShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&lightShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&illuminatedShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);

    switch(currentScene){

        case MapScene:

        break;
        case Battle:
        break;
    }

    BeginShaderMode(oceanShader.shader);
    DrawRectangle(0, 0, WIDTH, HEIGHT, BLACK);
    
}
void EndOceanPass(){
    EndShaderMode();
}
void PrepShipRangePass(){
    switch (currentScene) {
        case Menu: {
        break;
        }
        case MapScene:{
            DotShaderValues(&oceanShader, 0.1, 100, (Vector3){0,0, 0});
        }
        break;

        case Battle:{
            DotShaderValues(&oceanShader, 0.07, 250, (Vector3){0,0, 0});
        }
        break;
    }
    BeginShaderMode(oceanShader.shader);
}

void UnloadShaders(){
    UnloadShader(islandShader.shader);
    UnloadShader(oceanShader.shader);
    UnloadShader(explosionShader.shader);
    UnloadShader(generalShader.shader);
    UnloadShader(lightShader.shader);
    UnloadShader(illuminatedShader.shader);
    UnloadShader(skyShader.shader);
}