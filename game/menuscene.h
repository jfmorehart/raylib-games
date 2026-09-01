#include "filesystem.h"
#include "globals.h"
#include "UI.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cutscene.h"

Font menufont;

int currentChar;
#define MAXCHARS 2048

char textBuffer[MAXCHARS];
float last_delete_time;
float delete_cooldown = 0.1;
bool help;

typedef struct DisplayLine{
    int length;
    char lineBuffer[MAXCHARS];
    int renderedCharCount; //if this == length, we're done.
}DisplayLine;


typedef enum Campaign{
    Raenin, 
    Lutzo, 
    Kenning, 
    Artem
}Campaign;
Campaign selected;

char displayBuffer[MAXCHARS];
int displayBufferCham = 0;
float lastWriteTime;

int backlogLength = 10;
DisplayLine backLog[10];
int backLogCham;

extern Map mapFromDisk;

int AddToBackLog(char *toadd, int length){
    char * writeto = backLog[backLogCham].lineBuffer;
    char * read = toadd;
    printf("writing %s to: %s\n",read, writeto);

    strncpy(writeto, read, strlen(read));
    printf("now reads: %s\n", writeto);

    backLog[backLogCham].lineBuffer[strlen(toadd)] = '\0';
    backLog[backLogCham].length = length;
    backLog[backLogCham].renderedCharCount = 0;
    int index = backLogCham;
    backLogCham++;
    if(backLogCham >= backlogLength) backLogCham = 0;
    return index;
}
void AddLine(char *toadd){
    int len = strlen(toadd);
    int index = AddToBackLog(toadd, len);
    backLog[index].lineBuffer[len] = '\n';
    backLog[index].lineBuffer[len + 1] = '\0';
    backLog[index].length++;
}


void MenuInit(){
    memset(textBuffer, 0, sizeof(char) * MAXCHARS);
    currentChar = 0;
    memset(displayBuffer, 0, sizeof(char) * MAXCHARS);
    backLogCham = 0;
    displayBufferCham = 0;
    printf("loading menu\n");

    // const char* hello = "hello";
    // printf("strlen hello = %d\n", strlen(hello));

    menufont = LoadFont("assets/jackinput.ttf");
    currentChar = 0;
    memset(textBuffer, 0, sizeof(textBuffer));
    SetExitKey(0);
    help = false;

    AddLine("remaining names:");
    AddLine("Raenin, Lutzo, Kenning, Artem");
}


void WriteBacklogToDisplayBuffer_Tick(){
    for(int i = 0; i < backlogLength; i++){
        if(backLog[i].renderedCharCount < backLog[i].length){
            //this is our next render!
            displayBuffer[displayBufferCham] = backLog[i].lineBuffer[backLog[i].renderedCharCount];
            displayBufferCham++;
            if(displayBufferCham >= MAXCHARS) displayBufferCham = 0;
            backLog[i].renderedCharCount++;
            return;
        }
    }
}

float lastBlink = 0;
bool blinkON;
void MenuUpdate(){

    ClearBackground(BLACK);

    if(unscaledTime - lastWriteTime > 0.01){
        WriteBacklogToDisplayBuffer_Tick();
        lastWriteTime = unscaledTime;
    }

    if(unscaledTime - lastBlink > 0.4){
        lastBlink = unscaledTime;
        blinkON = !blinkON;
    }

    int key = GetCharPressed();

    while (key > 0){
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (currentChar < MAXCHARS - 1))
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
    DrawText("SAVO_C            __ development version 0.17 __", 100, 30, 30, DARKGRAY);
    DrawText("made by spacemann", WIDTH * RSCALE - 260, HEIGHT * RSCALE - 30, 20, DARKGRAY);
    DrawText("menu commands: edit, quit, help, cut, sink", 100, 60, 30, DARKGRAY);

    if(help){
        DrawText("play: M1 -> select, M2 -> order. Space -> next day. P -> editor", 100, RSCALE *HEIGHT/3 + 50, 30, GRAY);
        DrawText("edit: E -> wind: (Click -> new vertex) P -> place: (JKL; -> +ship). S to save.", 100, RSCALE *HEIGHT/3 + 100, 30, GRAY);
    }

    if(strcmp(textBuffer, "Raenin") == 0){
        selected = Raenin;

        memset(textBuffer, 0, sizeof(textBuffer));
        currentChar = 0;
        memset(displayBuffer, 0, sizeof(displayBuffer));
        displayBufferCham = 0;
        AddLine("Admiral Wilhelm Raenin.");
        AddLine("You will be executed in 87 days.");
        AddLine("At your disposal are the battleships:");
        AddLine("Scharnitz");
        AddLine("Tirphorst");
        AddLine("and nine destroyers");
        AddLine("Begin? (y/n)");
    }
    
    if(strcmp(textBuffer, "Lutzo") == 0){
        selected = Lutzo;
        memset(textBuffer, 0, sizeof(textBuffer));
        currentChar = 0;
        memset(displayBuffer, 0, sizeof(displayBuffer));
        displayBufferCham = 0;
        AddLine("Lutzo.");
        AddLine("You will not survive the war.");
        AddLine("Begin? (y/n)");
    }

    if(strcmp(textBuffer, "Kenning") == 0){
        selected = Kenning;

        memset(textBuffer, 0, sizeof(textBuffer));
        currentChar = 0;
        memset(displayBuffer, 0, sizeof(displayBuffer));
        displayBufferCham = 0;
        AddLine("Lutzo.");
        AddLine("You will not survive the war.");
        AddLine("Begin? (y/n)");
    }
    if(strcmp(textBuffer, "Artem") == 0){
        selected = Lutzo;
        memset(textBuffer, 0, sizeof(textBuffer));
        currentChar = 0;
        memset(displayBuffer, 0, sizeof(displayBuffer));
        displayBufferCham = 0;
        AddLine("Lutzo.");
        AddLine("You will not survive the war.");
        AddLine("Begin? (y/n)");
    }

    if(strcmp(textBuffer, "yes") == 0){
        memset(textBuffer, 0, sizeof(textBuffer));
        currentChar = 0;
        memset(displayBuffer, 0, sizeof(displayBuffer));
        displayBufferCham = 0;
        mapFromDisk = LoadMapFile("bergen.campaign");
        SwitchScenes(MapScene);
    }

    if(strcmp(textBuffer, "capt") == 0){
        memset(textBuffer, 0, MAXCHARS);
        currentChar = 0;
        memset(displayBuffer, 0, MAXCHARS);
        displayBufferCham = 0;
        SetCutscene(LoneCaptain);
        SwitchScenes(CutScene);
    }

    DrawText(displayBuffer, 100, RSCALE * HEIGHT / 3, 30, WHITE);
    if(blinkON){
        DrawText(">", 80, RSCALE * HEIGHT - 100, 30, WHITE);
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
    if(strcmp(textBuffer, "sink") == 0){
        SetCutscene(SinkingEnemy);
        SwitchScenes(CutScene);
    }
    if(strcmp(textBuffer, "help") == 0){
        help = true;
        currentChar = 0;
        memset(textBuffer, 0, sizeof(textBuffer)); 
    }
}