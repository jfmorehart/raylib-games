#pragma once
#include <raylib.h>


float worldScale;

int WIDTH;
int HEIGHT;

Vector2 cameraPosition;
Vector2 screenVec;
Vector2 mousePos;
Vector2 xBounds;
Vector2 yBounds;

#define ISLANDCOUNT  40
#define MAX_SHIPS 10
#define FRAMERATE 120
int frameCount;
float frames_fudged;

float scaledDeltaTime;
float fixedDeltaTime;
float scaledTime;
float unscaledTime;
float timeScale;

typedef enum SceneName{
    Menu,
    Map,
    Battle
}SceneName;
SceneName currentScene;

typedef struct Scene{
    SceneName name;
    void (*RunOnInit)(void);
}Scene;


#define SCENECOUNT 3
Scene scenes[SCENECOUNT];


