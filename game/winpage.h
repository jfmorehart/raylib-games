#pragma once

#include "globals.h"
#include "helpers.h"
#include "islands.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "filesystem.h"
#include "UI.h"
#include "shiploadouts.h"
#include <string.h>
#include "mapshaders.h"
#include "rlgl.h"
#include "cutscene.h"

/*
The point of this scene is to familiarize the player with the ships and captains and men that they are about to put at risk!
We want the player to feel some sense of responsibility here, as well as a kind of vague impending doom

These ships were built with pride, for an empire or kingdom. These men are partially volunteers, partially conscripted. 
They may not all be happy to be in these ships, but they all more or less believe in the cause, and your ability to lead them. 
You are a senior admiral, and responsible for their lives.
All of these ships will sink. All of these men will die. 

*/

extern DotShader map_islandShader;
extern DotShader generalShader;

extern Map mapFromDisk;
extern Vector2 worldZero;

typedef enum IntroState{
    Impetus, 
    Mission,
    Ships
}IntroState;
IntroState state;

void RenderShipTroopScreen(Vector2 screenPos, Ship ship){

    DrawText(ship.shipName, screenPos.x - 100, screenPos.y + 55, 20, WHITE);//, int fontSize, Color color)
    DrawText(ship.captName, screenPos.x - 100, screenPos.y + 85, 15, GRAY);//, int fontSize, Color color)

    Vector3 col = (Vector3){1, 1, 1};
    DotShaderValues(&generalShader, 0.2, 12, col);
    SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);

    BeginShaderMode(generalShader.shader);
    rlBegin(RL_TRIANGLES);
    rlColor4ub(255, 255, 255, 255);

    RenderShipIconAtPoint(&ship, 90, screenPos);//, int polyScale, Vector2 point)
    // cruiser.polyCenter = screenPos;
    // cruiser.polyScale = 90;
    // RenderPolyAsUI(cruiser);

    rlEnd();
    rlSetTexture(0); 
    EndShaderMode();
}

void DrawButton(char * text, Vector2 buttonTL, IntroState toset){

    DrawRectangle(buttonTL.x, buttonTL.y, 60, 40, WHITE);
    DrawRectangle(buttonTL.x + 2.5,buttonTL.y + 2.5, 54, 36, BLACK);

    if(mousePos_UIScreenCoords.x > buttonTL.x && mousePos_UIScreenCoords.x < buttonTL.x + 60 && mousePos_UIScreenCoords.y > buttonTL.y && mousePos_UIScreenCoords.y < buttonTL.y + 40){
        DrawText(text, buttonTL.x + 5, buttonTL.y + 10, 20, WHITE);//, int fontSize, Color color)
        if(IsMouseButtonDown(0)){
            if(state == toset){
                SwitchScenes(MapScene);
            }else{
                state = toset;
            }
        }
    }else{
        DrawText(text, buttonTL.x + 5, buttonTL.y + 10, 20, GRAY);//, int fontSize, Color color)
    }

}

void ShipsUnderCommand(){

    DrawText("SHIPS SURVIVING", 100, 100, 60, WHITE);//, int fontSize, Color color)

    int columns = 3;
    int rows = 3;

    for(int i = 0; i < mapFromDisk.fcount; i++){
        float x = i % columns;
        float y = floorf(i / columns);
        Vector2 screenPos = (Vector2){0.8 * WIDTH * (x / columns) * RSCALE + WIDTH * 0.15 * RSCALE, HEIGHT * (y / rows) * RSCALE * 0.8 + HEIGHT * 0.3 * RSCALE};
        RenderShipTroopScreen(screenPos, mapFromDisk.friendlies[i]);
    }

    Vector2 buttonTL = (Vector2){RSCALE * WIDTH * 0.9, RSCALE * HEIGHT * 0.8};
    DrawButton("Next", buttonTL, Ships);
    buttonTL.y += 80;
    DrawButton("Back", buttonTL, Mission);
}

void RenderReport(){
    DrawText("MISSION COMPLETED", WIDTH * RSCALE * 0.2, 100, 60, WHITE);//, int fontSize, Color color)

    Vector2 buttonTL = (Vector2){RSCALE * WIDTH * 0.9, RSCALE * HEIGHT * 0.5};
    DrawButton("Next", buttonTL, Ships);
    buttonTL.y += 80;
    DrawButton("Back", buttonTL, Impetus);
}

void TroopUIUpdate(){

    //render a couple ships at a time and a way to scroll between them

    //an icon of the ship
    //name of the ship
    //captain of the ship
    //speed, armamanents, 
    //total souls aboard

    switch(state){
        case Impetus:
        break;
        case Mission:
            RenderReport();
        break;
        case Ships:
            ShipsUnderCommand();
        break;

    }
    // printf("troop scene update!\n");
}