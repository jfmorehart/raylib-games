#include "globals.h"
#include "UI.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Font menufont;

int currentChar;
#define MAXCHARS 512

char textBuffer[MAXCHARS];
float last_delete_time;
float delete_cooldown = 0.1;
bool help;

void MenuInit(){
    printf("loading menu\n");
    menufont = LoadFont("assets/jackinput.ttf");
    currentChar = 0;
    memset(textBuffer, 0, sizeof(textBuffer));
    SetExitKey(0);
    help = false;
}

void MenuUpdate(){

    ClearBackground(BLACK);

    int key = GetCharPressed();

    while (key > 0){
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (currentChar < MAXCHARS))
        {
            textBuffer[currentChar] = (char)key;
            textBuffer[currentChar+1] = '\0'; // Add null terminator at the end of the string
            currentChar++;
        }

        key = GetCharPressed();  // Check next character in the queue 
    }
    if (IsKeyDown(KEY_BACKSPACE) && unscaledTime - last_delete_time > delete_cooldown)
    {
        last_delete_time = unscaledTime;
        currentChar--;
        if (currentChar < 0) currentChar = 0;
        textBuffer[currentChar] = '\0';
    }
    // printf("%s", buffer);
    DrawText("SAVO_C            __ development version 0.15 __", 100, 30, 30, GRAY);
    DrawText("made by spacemann", WIDTH * RSCALE - 260, HEIGHT * RSCALE - 30, 20, GRAY);

    DrawText("menu commands: play, edit, quit, help", 100, RSCALE *HEIGHT/3, 30, GRAY);

    if(help){
        DrawText("play: M1 -> select, M2 -> order. Space -> next day. P -> editor", 100, RSCALE *HEIGHT/3 + 50, 30, GRAY);
        DrawText("edit: E -> wind: (Click -> new vertex) P -> place: (JKL; -> +ship). S to save.", 100, RSCALE *HEIGHT/3 + 100, 30, GRAY);
    }

    DrawText(textBuffer, 100, RSCALE * HEIGHT - 100, 30, GRAY);

    if(strcmp(textBuffer, "cut") == 0){
        SwitchScenes(CutScene);
    }
    if(strcmp(textBuffer, "play") == 0){
        SwitchScenes(MapScene);
    }
    if(strcmp(textBuffer, "edit") == 0){
        SwitchScenes(Editor);
    }
    if(strcmp(textBuffer, "quit") == 0){
        exit(0);
    }
    if(strcmp(textBuffer, "help") == 0){
        help = true;
        currentChar = 0;
        memset(textBuffer, 0, sizeof(textBuffer)); 
    }
}

/*         // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                {
                    name[letterCount] = (char)key;
                    name[letterCount+1] = '\0'; // Add null terminator at the end of the string
                    letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                name[letterCount] = '\0';
            }*/