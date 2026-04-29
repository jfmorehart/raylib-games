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

void RenderWindow(Window *window, Font *font);