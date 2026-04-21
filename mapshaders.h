#pragma once
#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdlib.h>
#include "globals.h"

Shader islandShader_frag;
Shader oceanShader_frag;


int timeLoc;
int landTime;


int ShaderInit(){

    islandShader_frag = LoadShader(0, "island.fs");
    oceanShader_frag = LoadShader(0, "ocean.fs");

    //set RESOLUTION
    int resLoc = GetShaderLocation(islandShader_frag, "resolution");        
    float res[2] = { (float)WIDTH, (float)HEIGHT };                                                                                                                                                    
    SetShaderValue(islandShader_frag, resLoc, res, SHADER_UNIFORM_VEC2);     

    resLoc = GetShaderLocation(oceanShader_frag, "resolution");                                                                                                                                               
    SetShaderValue(oceanShader_frag, resLoc, res, SHADER_UNIFORM_VEC2);     


    //setup ISLAND CONSTANTS
    resLoc = GetShaderLocation(islandShader_frag, "multiplier");   
    int multiplier = 80;
    SetShaderValue(islandShader_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    resLoc = GetShaderLocation(islandShader_frag, "dotsize");   
    float dotsize = 0.1;
    SetShaderValue(islandShader_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    return 0;
}

void PrepOceanPass(Vector2 mousePos, int multiplier, float dotsize){

    int mousePosLoc = GetShaderLocation(oceanShader_frag, "mpos");   

    //setup OCEAN CONSTANTS
    int resLoc = GetShaderLocation(oceanShader_frag, "multiplier");   
    SetShaderValue(oceanShader_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    resLoc = GetShaderLocation(oceanShader_frag, "dotsize");   
    SetShaderValue(oceanShader_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    timeLoc = GetShaderLocation(oceanShader_frag, "_Time"); 
    landTime = GetShaderLocation(islandShader_frag, "_Time"); 

    SetShaderValue(oceanShader_frag, mousePosLoc, &mousePos, SHADER_UNIFORM_VEC2);
    SetShaderValue(oceanShader_frag, timeLoc, &unscaledTime, SHADER_UNIFORM_FLOAT);
    SetShaderValue(islandShader_frag, landTime, &unscaledTime, SHADER_UNIFORM_FLOAT);

    resLoc = GetShaderLocation(oceanShader_frag, "worldScale");   
    SetShaderValue(oceanShader_frag, resLoc, &worldScale, SHADER_UNIFORM_FLOAT);
    resLoc = GetShaderLocation(oceanShader_frag, "cameraPosition");   
    SetShaderValue(oceanShader_frag, resLoc, &cameraPosition, SHADER_UNIFORM_VEC2);

    BeginShaderMode(oceanShader_frag);
    
    DrawRectangle(0, 0, WIDTH, HEIGHT, BLACK);

    resLoc = GetShaderLocation(islandShader_frag, "worldScale");   
    SetShaderValue(islandShader_frag, resLoc, &worldScale, SHADER_UNIFORM_FLOAT);
    resLoc = GetShaderLocation(islandShader_frag, "cameraPosition");   
    SetShaderValue(islandShader_frag, resLoc, &cameraPosition, SHADER_UNIFORM_VEC2);


}
void EndOceanPass(){
    EndShaderMode();
}
void PrepShipRangePass(){
    int resLoc = GetShaderLocation(oceanShader_frag, "multiplier");   
    int multiplier = 60;
    SetShaderValue(oceanShader_frag, resLoc, &multiplier, SHADER_UNIFORM_INT);

    resLoc = GetShaderLocation(oceanShader_frag, "dotsize");   
    float dotsize = 0.04;
    SetShaderValue(oceanShader_frag, resLoc, &dotsize, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(oceanShader_frag);
}

void UnloadShaders(){
    UnloadShader(islandShader_frag);
    UnloadShader(oceanShader_frag);
}