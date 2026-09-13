#pragma once
#include <raylib.h>

float worldScale;

int WIDTH;
int HEIGHT;

Vector2 cameraPosition;
Vector2 screenVec;
Vector2 mousePos;
Vector2 mousePos_ScreenCoords;
Vector2 mousePos_UIScreenCoords;
Vector2 mousePos_fragCoords;

Vector2 xBounds;
Vector2 yBounds;


#define RSCALE 1.5


#define ISLANDCOUNT  40
#define MAX_SHIPS 20
#define FRAMERATE 120

#define TF_MAX_RADIUS 0.3
#define MAX_SHIPS_IN_TF 10
#define MAX_TFS 6

int frameCount;
float frames_fudged;

float scaledDeltaTime;
float fixedDeltaTime;
float scaledTime;
float unscaledTime;
float timeScale;
float worldTime;

#define BATTLESCENE_SPEEDMULT 0.08


typedef enum SceneName{
    Menu,
    MapScene,
    Battle,
    CutScene,
    Editor,
    TroopScene
}SceneName;
SceneName currentScene;

typedef struct Scene{
    SceneName name;
    void (*RunOnInit)(void);
}Scene;


#define SCENECOUNT 6
Scene scenes[SCENECOUNT];

Vector2 destOffset;


