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
#include "text.h"
#include "progression.h"

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
extern PolyPoly cruiser;

extern Map mapFromDisk;
extern Vector2 worldZero;

char captname[64];

typedef enum IntroState{
    Impetus, 
    Mission,
    Ships
}IntroState;
IntroState state;

TextBuffer text;
int textCharsToRender;
char tempTextBuffer[MAXBUFFERLENGTH];

void SwitchPage(IntroState to){
    switch(to){
        case Impetus:
            textCharsToRender = 0;
            ClearBuffer(&text);
            AddBufferText(&text, GetEnemyActionText());
        break;
        case Mission:
            textCharsToRender = 0;
            ClearBuffer(&text);
            AddBufferText(&text, GetMissionText());
            // AddBufferText(&text, "");
        break;
        case Ships:
        break;
    }
}

void InitTroopScene(){
    state = Impetus;
    SwitchPage(Impetus);

    ClearBackground(BLACK);

    worldScale = 2;
    cameraPosition = worldZero;
    FrameRefreshShader(&generalShader, unscaledTime, cameraPosition, worldScale, worldZero);
    // CreateCaptainName_NonAlloc(captname);

    for(int i = 0; i < mapFromDisk.fcount; i++){
        InitRvecs(&mapFromDisk.friendlies[i]);
    }
    for(int i = 0; i < mapFromDisk.ecount; i++){
        InitRvecs(&mapFromDisk.enemies[i]);
    }
}


void RenderShipTroopScreen(Vector2 screenPos, Ship ship){

    //render a couple ships at a time and a way to scroll between them

    //an icon of the ship
    //name of the ship
    //captain of the ship
    //speed, armamanents, 
    //total souls aboard
    int textLROffset = 70;
    switch(ship.shipType){
        case Battleship:
            DrawText("Battleship", screenPos.x - textLROffset, screenPos.y - 70, 15, GRAY);//, int fontSize, Color color)
        break;
        case Destroyer:
            DrawText("Destroyer", screenPos.x - textLROffset, screenPos.y -70, 15, GRAY);//, int fontSize, Color color)
        break;
        case Transport:
            DrawText("Transport", screenPos.x - textLROffset, screenPos.y -70, 15, GRAY);//, int fontSize, Color color)
        break;
    }

    DrawText(ship.shipName, screenPos.x - textLROffset, screenPos.y + 55, 20, WHITE);//, int fontSize, Color color)
    DrawText(ship.captName, screenPos.x - textLROffset, screenPos.y + 85, 15, GRAY);//, int fontSize, Color color)
    switch(ship.shipType){
        case Battleship:
        DrawText("2,500 men", screenPos.x - textLROffset, screenPos.y + 105, 15, DARKGRAY);//, int fontSize, Color color)

        if(ship.health > BattleshipStats.health * 0.5 && ship.health < BattleshipStats.health){
            DrawText("lightly damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
        }else if (ship.health < BattleshipStats.health * 0.5){
            DrawText("critically damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
        }

        break;
        case Destroyer:
            DrawText("300 men", screenPos.x - textLROffset, screenPos.y + 105, 15, DARKGRAY);//, int fontSize, Color color)
            if(ship.health > DestroyerStats.health * 0.5 && ship.health < DestroyerStats.health){
                DrawText("lightly damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
            }else if (ship.health < DestroyerStats.health * 0.5){
                DrawText("critically damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
            }
        break;

        case Transport:
            DrawText("300 men", screenPos.x - textLROffset, screenPos.y + 105, 15, DARKGRAY);//, int fontSize, Color color)
            if(ship.health > TransportStats.health * 0.5 && ship.health < TransportStats.health){
                DrawText("lightly damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
            }else if (ship.health < TransportStats.health * 0.5){
                DrawText("critically damaged", screenPos.x - textLROffset, screenPos.y + 125, 15, DARKGRAY);//, int fontSize, Color color)
            }
        break;
    }
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
                SwitchPage(toset);
            }
        }
    }else{
        DrawText(text, buttonTL.x + 5, buttonTL.y + 10, 20, GRAY);//, int fontSize, Color color)
    }

}

void ShipsUnderCommand(){


    DrawText("SHIPS UNDER YOUR COMMAND", 100, 100, 60, WHITE);//, int fontSize, Color color)

    int columns = 3;
    int rows = 3;

    for(int i = 0; i < mapFromDisk.fcount; i++){
        float x = i % columns;
        float y = floorf(i / columns);
        Vector2 screenPos = (Vector2){0.8 * WIDTH * (x / columns) * RSCALE + WIDTH * 0.15 * RSCALE, HEIGHT * (y / rows) * RSCALE * 0.8 + HEIGHT * 0.3 * RSCALE};
        RenderShipTroopScreen(screenPos, mapFromDisk.friendlies[i]);
    }


    // screenPos.x += WIDTH * 0.3 * RSCALE;
    // RenderShipTroopScreen(screenPos, mapFromDisk.friendlies[1]);
    // screenPos.x += WIDTH * 0.3 * RSCALE;
    // RenderShipTroopScreen(screenPos, mapFromDisk.friendlies[2]);
    

    Vector2 buttonTL = (Vector2){RSCALE * WIDTH * 0.9, RSCALE * HEIGHT * 0.8};
    DrawButton("Next", buttonTL, Ships);
    buttonTL.y += 80;
    DrawButton("Back", buttonTL, Mission);
}

void RenderImpetus(){
    DrawText("ENEMY ACTION", 100, 100, 60, WHITE);//, int fontSize, Color color)

    snprintf(tempTextBuffer, textCharsToRender, "%s", text.charArray);
    DrawText(tempTextBuffer, 100, 300, 30, GRAY);

    Vector2 buttonTL = (Vector2){RSCALE * WIDTH * 0.5, RSCALE * HEIGHT * 0.8};
    DrawButton("Next", buttonTL, Mission);
}

void RenderMission(){
    DrawText("MISSION", WIDTH * RSCALE * 0.7, 100, 60, WHITE);//, int fontSize, Color color)

    Vector2 buttonTL = (Vector2){RSCALE * WIDTH * 0.9, RSCALE * HEIGHT * 0.5};
    DrawButton("Next", buttonTL, Ships);
    buttonTL.y += 80;
    DrawButton("Back", buttonTL, Impetus);

    FrameRefreshShader(&map_islandShader, unscaledTime, cameraPosition, 1, worldZero);
    DotShaderValues(&map_islandShader, 0.04, 60, (Vector3){1, 1, 1});
    BeginShaderMode(map_islandShader.shader);
    
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES); 
    for(int i = 0; i < mapFromDisk.islandLength; i++){
        Render(&mapFromDisk.islands[i]);
    }
    rlEnd();
    rlSetTexture(0); 
    EndShaderMode();

    Vector3 col = (Vector3){1, 1, 1};
    DotShaderValues(&generalShader, 0.2, 120, col);
    SetShaderValue(generalShader.shader, generalShader.colLoc, &col, SHADER_UNIFORM_VEC3);

    BeginShaderMode(generalShader.shader);
    rlSetTexture(rlGetTextureIdDefault());                                                                                 
    rlBegin(RL_TRIANGLES); 
    for(int f = 0; f < mapFromDisk.fcount; f++){
        RenderShipIconAtPoint(&mapFromDisk.friendlies[f], 10, WorldToScreen(mapFromDisk.friendlies[f].wPos));
    }
    rlEnd();
    rlSetTexture(0); 
    EndShaderMode();

    float border = 30;
    float topinlay = 30;
    float diff = 0.4 * (WIDTH - HEIGHT);
    
    DrawRectangle(0, 0, WIDTH, topinlay, BLACK);//top
    DrawRectangle(0, 0, diff, HEIGHT, BLACK); //left

    DrawRectangle(0, HEIGHT - topinlay, WIDTH, topinlay + border, BLACK); // bottom
    DrawRectangle(WIDTH - diff, 0, diff, HEIGHT, BLACK); //right

    DrawRectangle(diff, topinlay, WIDTH - diff * 2, 5, GRAY); //top
    DrawRectangle(diff, topinlay, 5, HEIGHT- topinlay * 2, GRAY); // left?

    DrawRectangle(diff, HEIGHT - topinlay, WIDTH - diff * 2, 5, GRAY); //bottom
    DrawRectangle(WIDTH - diff, topinlay, 5, HEIGHT - topinlay * 2, GRAY); //right

    snprintf(tempTextBuffer, textCharsToRender, "%s", text.charArray);
    DrawText(tempTextBuffer, 100, HEIGHT * RSCALE * 0.7, 30, GRAY);

}

float lastTextTick;
float textTickDelay =0.03;

void TroopUIUpdate(){

    if(unscaledTime - lastTextTick > textTickDelay){
        if(textCharsToRender < text.cham){
            textCharsToRender++;
            lastTextTick = unscaledTime;
        }
    }

    switch(state){
        case Impetus:
            RenderImpetus();
        break;
        case Mission:
            RenderMission();
        break;
        case Ships:
            ShipsUnderCommand();
        break;

    }
    // printf("troop scene update!\n");
}