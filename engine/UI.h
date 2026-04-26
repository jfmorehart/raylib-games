#pragma once

#include <raylib.h>

#include "globals.h"

typedef enum Corner{
    TOP_LEFT, 
    TOP_RIGHT,
    BOTTOM_LEFT, 
    BOTTOM_RIGHT
}Corner;

typedef struct Window{
    Corner corner;
    Vector2 pixelScale;
    const char* header;
    int headerFontSize;
    const char* body;
    int bodyFontSize;
} Window;

void RenderWindow(Window *window, Font *font){
    int xPos;
    int yPos;
    switch (window->corner) {
        case TOP_LEFT:
            xPos = 0;
            yPos = 0;
        break;
        case TOP_RIGHT:
            xPos = WIDTH - window->pixelScale.x;
            yPos = 0;
        break;
        case BOTTOM_LEFT:
            xPos = 0;
            yPos = window->pixelScale.y;
        break;
        case BOTTOM_RIGHT:
            xPos = WIDTH - window->pixelScale.x;
            yPos = HEIGHT - window->pixelScale.y;
    }
    Vector2 pos = {xPos + window->pixelScale.x * 0.2, yPos + window->pixelScale.y * 0.1};
    DrawRectangle(xPos, yPos, window->pixelScale.x, window->pixelScale.y, GRAY);
    DrawTextEx(*font, window->header, pos, window->headerFontSize, 4, WHITE);
    pos = (Vector2){xPos + window->pixelScale.x * 0.1, yPos + window->pixelScale.y * 0.4};
    DrawTextEx(*font, window->body, pos, window->bodyFontSize, 4, WHITE);
}