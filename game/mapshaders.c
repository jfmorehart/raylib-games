#include "mapshaders.h"     
#include <stdlib.h>
#include "globals.h"

DotShader CreateDotShader(Shader shader){
    //unified system for storing the shaderlocations in memory
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

//pass in important info for recreating worldspace
void FrameRefreshShader(DotShader *ds, float time, Vector2 camPos, float worldScale, Vector2 special_Mpos){
    SetShaderValue(ds->shader, ds->tloc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->wsLoc, &worldScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->camLoc, &camPos, SHADER_UNIFORM_VEC2); 
    SetShaderValue(ds->shader, ds->mposLoc, &special_Mpos, SHADER_UNIFORM_VEC2); 
}
//pass in basics
void DotShaderValues(DotShader *ds, float dotSize, int multiplier, Vector3 color){
    SetShaderValue(ds->shader, ds->dloc, &dotSize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ds->shader, ds->multLoc, &multiplier, SHADER_UNIFORM_INT);
    SetShaderValue(ds->shader, ds->colLoc, &color, SHADER_UNIFORM_VEC3);
}

DotShader islandShader;
DotShader oceanShader;
DotShader generalShader;
DotShader generalShader_noCamMovement;
DotShader explosionShader;
DotShader lightShader;
DotShader illuminatedShader;
DotShader skyShader;
DotShader waterShader;
DotShader map_islandShader;
DotShader cut_foamShader;

extern SceneName currentScene;

int ShaderInit(){

    //load shaders into memory and compile
    islandShader = CreateDotShader(LoadShader(0, "shaders/island.fs"));
    oceanShader = CreateDotShader(LoadShader(0, "shaders/ocean.fs"));
    generalShader = CreateDotShader(LoadShader(0, "shaders/generaldot.fs"));
    generalShader_noCamMovement = CreateDotShader(LoadShader(0, "shaders/generaldot.fs"));
    explosionShader= CreateDotShader(LoadShader(0, "shaders/explosion.fs"));
    lightShader = CreateDotShader(LoadShader("shaders/beam.vs","shaders/beam.fs"));
    illuminatedShader = CreateDotShader(LoadShader(0,"shaders/illum.fs")); 
    skyShader = CreateDotShader(LoadShader(0,"shaders/sky.fs")); 
    waterShader = CreateDotShader(LoadShader(0,"shaders/water.fs")); 
    map_islandShader = CreateDotShader(LoadShader(0,"shaders/map_islands.fs")); 
    cut_foamShader = CreateDotShader(LoadShader(0,"shaders/cut_foamShader.fs"));

    //init with some values -- most places this sort of thing gets reset every frame, its not critical to do it here
    DotShaderValues(&generalShader, 0.2, 50, (Vector3){1, 1, 1});
    DotShaderValues(&generalShader_noCamMovement, 0.2, 50, (Vector3){1, 1, 1});
    DotShaderValues(&explosionShader, 0.3, 230, (Vector3){1, 1, 1});
    DotShaderValues(&lightShader, 0.2, 230, (Vector3){0.2, 0.2, 0.2});
    
    //pass in screen information
    Vector2 resolutionVector = {WIDTH, HEIGHT};  
    SetRes(&islandShader, resolutionVector);
    SetRes(&oceanShader, resolutionVector);
    SetRes(&generalShader, resolutionVector);
    SetRes(&generalShader_noCamMovement, resolutionVector);
    SetRes(&explosionShader, resolutionVector); 
    SetRes(&lightShader, resolutionVector); 
    SetRes(&illuminatedShader, resolutionVector); 
    SetRes(&skyShader, resolutionVector); 
    SetRes(&waterShader, resolutionVector); 
    SetRes(&map_islandShader, resolutionVector); 
    SetRes(&cut_foamShader, resolutionVector);

    return 0;
}

void PrepOceanPass(Vector2 specialMousePos, int multiplier, float dotsize){

    //pass in important info for recreating worldspace

    //setup OCEAN CONSTANTS
    DotShaderValues(&oceanShader, dotsize, multiplier, (Vector3){0 ,0, 0});
    FrameRefreshShader(&oceanShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&islandShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&explosionShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&lightShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&illuminatedShader, unscaledTime, cameraPosition,  worldScale, specialMousePos);
    FrameRefreshShader(&map_islandShader, unscaledTime, cameraPosition, worldScale, specialMousePos);
    FrameRefreshShader(&cut_foamShader, unscaledTime, cameraPosition, worldScale, specialMousePos);//, Vector2 camPos, float worldScale, Vector2 special_Mpos)

    BeginShaderMode(oceanShader.shader);
    DrawRectangle(0, 0, WIDTH, HEIGHT, BLACK);
}
void EndOceanPass(){
    EndShaderMode();
}
void PrepShipRangePass(){
    //decide scale of the dots for the shiprange shader
    switch (currentScene) {
        case Menu: {
        break;
        }
        case MapScene:{
            DotShaderValues(&oceanShader, 0.1, 100, (Vector3){0,0, 0});
        }
        break;
        case Battle:{
            DotShaderValues(&oceanShader, 0.03, 250, (Vector3){0,0, 0});
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
    UnloadShader(waterShader.shader);
    UnloadShader(cut_foamShader.shader);
}