#include "raylib.h"
#include "raymath.h"
#include <math.h>       
#include <stdlib.h>
#include "globals.h"

Shader islandShader_frag;
Shader oceanShader_frag;
Shader ship_frag;

int loc_ocean_time;
int loc_ocean_dot;
int loc_ocean_camPos;
int loc_ocean_worldScale;
int loc_ocean_mult;
int loc_ocean_res;
int loc_ocean_mpos;


int loc_land_time;
int loc_land_res;
int loc_land_mult;
int loc_land_dot;
int loc_land_worldScale;
int loc_land_camPos;

int loc_ship_res;
int loc_ship_mult;
int loc_ship_dot;
int loc_ship_worldScale;
int loc_ship_camPos;

extern SceneName currentScene;

int ShaderInit(){

    islandShader_frag = LoadShader(0, "shaders/island.fs");
    oceanShader_frag = LoadShader(0, "shaders/ocean.fs");
    ship_frag = LoadShader(0, "shaders/ship.fs");

    //FIND OCEAN UNIFORM LOCATIONS

    loc_ocean_res = GetShaderLocation(oceanShader_frag, "resolution");                                                                                                                                               
    loc_ocean_mpos = GetShaderLocation(oceanShader_frag, "mpos");   
    loc_ocean_mult= GetShaderLocation(oceanShader_frag, "multiplier"); 
    loc_ocean_dot = GetShaderLocation(oceanShader_frag, "dotsize");    
    loc_ocean_time = GetShaderLocation(oceanShader_frag, "_Time"); 
    loc_ocean_worldScale = GetShaderLocation(oceanShader_frag, "worldScale");   
    loc_ocean_camPos = GetShaderLocation(oceanShader_frag, "cameraPosition");   


    //FIND ISLAND UNIFORM LOCATIONS

    loc_land_res = GetShaderLocation(islandShader_frag, "resolution");        
    loc_land_dot= GetShaderLocation(islandShader_frag, "dotsize");   
    loc_land_mult = GetShaderLocation(islandShader_frag, "multiplier");   
    loc_land_time = GetShaderLocation(islandShader_frag, "_Time"); 
    loc_land_worldScale = GetShaderLocation(islandShader_frag, "worldScale");   
    loc_land_camPos = GetShaderLocation(islandShader_frag, "cameraPosition");   

    //FIND SHIP UNIFORM LOCATIONS

    loc_ship_res = GetShaderLocation(ship_frag, "resolution");
    loc_ship_dot = GetShaderLocation(ship_frag, "dotsize");   
    loc_ship_mult = GetShaderLocation(ship_frag, "multiplier");   
    loc_ship_worldScale= GetShaderLocation(ship_frag, "worldScale");   
    loc_ship_camPos = GetShaderLocation(ship_frag, "cameraPosition");  
    

    //setup ISLAND CONSTANTS
    int multiplier = 100;
    SetShaderValue(islandShader_frag, loc_land_mult, &multiplier, SHADER_UNIFORM_INT);

    float dotsize = 0.15;
    SetShaderValue(islandShader_frag, loc_land_dot, &dotsize, SHADER_UNIFORM_FLOAT);

    //setup Ship CONSTANTS
    multiplier = 50;
    SetShaderValue(ship_frag, loc_ship_mult, &multiplier, SHADER_UNIFORM_INT);
    dotsize = 0.2;
    SetShaderValue(ship_frag, loc_ship_dot, &dotsize, SHADER_UNIFORM_FLOAT);


    float resolutionVector[2] = {WIDTH, HEIGHT};  
    //set RESOLUTIONS
    SetShaderValue(islandShader_frag, loc_land_res, resolutionVector, SHADER_UNIFORM_VEC2);     
    SetShaderValue(oceanShader_frag, loc_ocean_res, resolutionVector, SHADER_UNIFORM_VEC2);     
    SetShaderValue(ship_frag, loc_ship_res, resolutionVector, SHADER_UNIFORM_VEC2);     
    return 0;
}

void PrepOceanPass(Vector2 mousePos, int multiplier, float dotsize){

    //setup OCEAN CONSTANTS
    SetShaderValue(oceanShader_frag, loc_ocean_mpos, &mousePos, SHADER_UNIFORM_VEC2);
    SetShaderValue(oceanShader_frag, loc_ocean_mult, &multiplier, SHADER_UNIFORM_INT);
    SetShaderValue(oceanShader_frag, loc_ocean_dot, &dotsize, SHADER_UNIFORM_FLOAT);

    SetShaderValue(oceanShader_frag, loc_ocean_time, &unscaledTime, SHADER_UNIFORM_FLOAT);
    SetShaderValue(oceanShader_frag, loc_ocean_worldScale, &worldScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(oceanShader_frag, loc_ocean_camPos, &cameraPosition, SHADER_UNIFORM_VEC2);

    //ship
    SetShaderValue(ship_frag, loc_ship_worldScale, &worldScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ship_frag, loc_ship_camPos, &cameraPosition, SHADER_UNIFORM_VEC2);

    //island
    SetShaderValue(islandShader_frag, loc_land_worldScale, &worldScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(islandShader_frag, loc_land_camPos, &cameraPosition, SHADER_UNIFORM_VEC2);
    SetShaderValue(islandShader_frag, loc_land_time, &unscaledTime, SHADER_UNIFORM_FLOAT);
    
    switch(currentScene){

        case MapScene:

        break;
        case Battle:
        break;
    }

    BeginShaderMode(oceanShader_frag);
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
            int multiplier = 100;
            SetShaderValue(oceanShader_frag, loc_ocean_mult, &multiplier, SHADER_UNIFORM_INT);

            float dotsize = 0.1;
            SetShaderValue(oceanShader_frag, loc_ocean_dot, &dotsize, SHADER_UNIFORM_FLOAT);
        }
        break;

        case Battle:{
             int multiplier = 250;
            SetShaderValue(oceanShader_frag, loc_ocean_mult, &multiplier, SHADER_UNIFORM_INT);

            float dotsize = 0.07;
            SetShaderValue(oceanShader_frag, loc_ocean_dot, &dotsize, SHADER_UNIFORM_FLOAT);
        }
        break;
    }
    BeginShaderMode(oceanShader_frag);
}


void UnloadShaders(){
    UnloadShader(islandShader_frag);
    UnloadShader(oceanShader_frag);
}