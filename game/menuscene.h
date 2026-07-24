#include "filesystem.h"
#include "globals.h"
#include "UI.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    memset(textBuffer, 0, MAXCHARS);
    currentChar = 0;
    memset(displayBuffer, 0, MAXCHARS);
    backLogCham = 0;
    displayBufferCham = 0;
    printf("loading menu\n");
    const char* hello = "hello";
    printf("strlen hello = %d\n", strlen(hello));
    menufont = LoadFont("assets/jackinput.ttf");
    currentChar = 0;
    memset(textBuffer, 0, sizeof(textBuffer));
    SetExitKey(0);
    help = false;

    AddLine("remaining names:");
    AddLine("Canaris, Raeber, Ciliax, Lutjens");
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


void MenuUpdate(){

    ClearBackground(BLACK);

    if(unscaledTime - lastWriteTime > 0.01){
        WriteBacklogToDisplayBuffer_Tick();
        lastWriteTime = unscaledTime;
    }

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
    DrawText("SAVO_C            __ development version 0.15 __", 100, 30, 30, DARKGRAY);
    DrawText("made by spacemann", WIDTH * RSCALE - 260, HEIGHT * RSCALE - 30, 20, DARKGRAY);
    DrawText("menu commands: play, cut, edit, quit, help", 100, 60, 30, DARKGRAY);

    if(help){
        DrawText("play: M1 -> select, M2 -> order. Space -> next day. P -> editor", 100, RSCALE *HEIGHT/3 + 50, 30, GRAY);
        DrawText("edit: E -> wind: (Click -> new vertex) P -> place: (JKL; -> +ship). S to save.", 100, RSCALE *HEIGHT/3 + 100, 30, GRAY);
    }

    if(strcmp(textBuffer, "Canaris") == 0){
        memset(textBuffer, 0, MAXCHARS);
        currentChar = 0;
        memset(displayBuffer, 0, MAXCHARS);
        displayBufferCham = 0;
        AddLine("Admiral Wilhelm Canaris. Abwehr, Reichsmarine.");
        AddLine("You will be executed in 87 days.");
        AddLine("At your disposal are the battleships:");
        AddLine("Scharnhorst");
        AddLine("Tirpitz");
        AddLine("and nine destroyers");
        AddLine("Begin?");
    }
    
    if(strcmp(textBuffer, "Raeber") == 0){
        memset(textBuffer, 0, MAXCHARS);
        currentChar = 0;
        memset(displayBuffer, 0, MAXCHARS);
        displayBufferCham = 0;
        AddLine("Raeber.");
        AddLine("You will survive the war.");
        AddLine("Begin?");
    }
    if(strcmp(textBuffer, "yes") == 0){
        memset(textBuffer, 0, MAXCHARS);
        currentChar = 0;
        memset(displayBuffer, 0, MAXCHARS);
        displayBufferCham = 0;
        mapFromDisk = LoadMapFile("bergen.campaign");
        SwitchScenes(MapScene);
    }

    DrawText(displayBuffer, 100, RSCALE * HEIGHT / 3, 30, WHITE);
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