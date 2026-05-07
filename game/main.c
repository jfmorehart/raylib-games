#include "battlescene.h"
#include "raylib.h"
#include "raymath.h"
#include "mapshaders.h"
#include "helpers.h"
#include "islands.h"
#include "globals.h"
#include "ships.h"
#include "UI.h"
#include "routines.h"
#include "editorscene.h"
#include "mapscene.h"
#include "menuscene.h"

#include <math.h>       
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Font font;
Window hello = {BOTTOM_RIGHT, {300, 150}, "Yooo", 32, "whats up", 12};
Vector2 worldZero;
RenderTexture2D targetTex;
Shader postProcess_frag;

AudioStream stream;
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
float buffer[BUFFER_SIZE];
void RunOnStart(){

    scenes[0] = (Scene){Menu, MenuInit};
    scenes[1] = (Scene){MapScene, InitMapScene};
    scenes[2] = (Scene){Battle, InitBattleScene};
    scenes[3] = (Scene){Editor, InitEditorScene};

    routines[0] = (Routine){"TimeRoutine", false, true, -999, 2, 2, TimeRoutine};
    routines[1] = (Routine){"FocusRoutine", false, true, -999, 1, 1, FocusRoutine};
    routines[2] = (Routine){"SwitchToBattleRoutine", false, true, -999, 1, 1, SwitchToBattleRoutine};
    routineCount = 3; //update with full number!

    InitWindow(WIDTH, HEIGHT, "raylib");
    
    ChangeDirectory(GetApplicationDirectory());
    font = LoadFont("assets/jackinput.ttf");
    postProcess_frag = LoadShader(0, "shaders/postprocess.fs");

    WIDTH = GetMonitorWidth(0);
    HEIGHT = GetMonitorHeight(0);

    screenVec = (Vector2){WIDTH, HEIGHT};
    SetTargetFPS(FRAMERATE);
    SetWindowSize(WIDTH, HEIGHT);


    WIDTH = WIDTH / RSCALE;
    HEIGHT = HEIGHT / RSCALE;
    targetTex = LoadRenderTexture(WIDTH, HEIGHT);
    SetTextureFilter(targetTex.texture, TEXTURE_FILTER_POINT);
    
    ToggleBorderlessWindowed();

    worldScale = 2;
    cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});    
    worldZero = cameraPosition;

    float diff = (WIDTH - HEIGHT) * 0.3;
    int border = 30;

    xBounds = (Vector2){ScreenToWorld((Vector2){diff, 0}).x, ScreenToWorld((Vector2){WIDTH - diff, 0}).x}; 
    yBounds = (Vector2){ScreenToWorld((Vector2){border, 0}).y, ScreenToWorld((Vector2){0, HEIGHT - border}).y}; 

    cameraPosition = ScreenToWorld((Vector2){WIDTH * 0.5, HEIGHT * 0.5});

    printf("xBounds = %.2f, %.2f\n", xBounds.x, xBounds.y);
    printf("yBounds = %.2f, %.2f\n", yBounds.x, yBounds.y);

    timeScale = 0.1;

    fixedDeltaTime = 1.0 /FRAMERATE;

    srand(1);//time(NULL));

    RandomizeMap();
    // InitMapScene();
    ShaderInit();

    SwitchScenes(MapScene);

    InitAudioDevice();

    // Set the number of samples the stream will keep in memory at a time to BUFFER_SIZE
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);

    // Init raw audio stream (sample rate: 44100, sample size: 32bit-float, channels: 1-mono)
    stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    float pan = 0.5;
    SetAudioStreamPan(stream, pan);
    PlayAudioStream(stream);
}

float sineFrequency = 440;
float phase = 0;
float sineStartTime = 0;
typedef enum WaveType{
    Sine, 
    Saw, 
    Triangle,
    Square,
    WhiteNoise,
    PerlinNoise,
    FBMNoise
}WaveType;

float nstep;
float CalcWave(WaveType wt, float phase){//FOR USE WITH NOISE FUNCITONS, PASS NSTEP * FREQ AS THE PHASE PARAM
    
    switch (wt){
        case Sine:
        return sin(phase);
        case Saw:
        return  (phase - PI) / PI;
        case Triangle:
        return   2 * ((fabsf(phase - PI) / PI) - 0.5);
        case Square:
        return 2 * (round(phase / (2 * PI)) - 0.5);
        case WhiteNoise:
        return (R01() * 2) - 1;
        case PerlinNoise:
        return Perlin(phase);
        case FBMNoise:
        return FBM(phase, 2, 0.5, 6);
    }
}

void ProcessAudio(){

    if (IsAudioStreamProcessed(stream))
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            int wavelength = SAMPLE_RATE/sineFrequency;
            
            nstep += 0.02; 
            buffer[i] = CalcWave(FBMNoise, nstep * 0.3 + 0.5 * sin(nstep * 0.01));

            phase += 2 * PI / wavelength;

            if (phase >= 2 * PI)
            {
                phase -= 2 * PI;
                sineStartTime = GetTime();
            }
        }

        UpdateAudioStream(stream, buffer, BUFFER_SIZE);
    }
}
int main(void)
{
    RunOnStart();
    while (!WindowShouldClose()) {

        ProcessAudio();
        BeginTextureMode(targetTex);

        scaledDeltaTime = GetFrameTime() * timeScale;
        scaledTime += scaledDeltaTime;
        unscaledTime += GetFrameTime();

        mousePos_ScreenCoords = GetMousePosition();
        mousePos_ScreenCoords = Vector2Scale(mousePos_ScreenCoords, 1.00 / RSCALE);       
        
        mousePos_UIScreenCoords = GetMousePosition();
        // printf("%f, %f, \n" ,   mousePos_UIScreenCoords.x,   mousePos_UIScreenCoords.y);
        mousePos = ScreenToWorld(mousePos_ScreenCoords);

        mousePos_fragCoords.y = HEIGHT * RSCALE - mousePos_ScreenCoords.y;
        mousePos_fragCoords = Vector2Scale(mousePos_fragCoords, 2 * RSCALE); 

        ExecuteRoutines();

        BeginDrawing();

        if(IsKeyPressed(KEY_ESCAPE)){
            SwitchScenes(Menu);
        }
        if(IsKeyPressed(KEY_P) && currentScene != Menu){
            SwitchScenes(Editor);
        }

        switch(currentScene){
            case Menu:

            break;
            case MapScene:
                worldTime += scaledDeltaTime;
                MapInputLoop();
                MapFrameLoop();
                break;
            case Battle:
                worldTime += scaledDeltaTime * 0.004;
                BattleFrameLoop();
            break;
            case Editor:
               EditorFrameLoop();
            break;
        }

        float diff = (WIDTH - HEIGHT) * 0.4;
        int border = 30;


        int grey = 50;
        Color outlineCol = (Color){grey, grey, grey, 255};
        DrawRectangle(0, 0, diff, HEIGHT, BLACK);//, int posY, int width, int height, Color color)
        DrawRectangle(WIDTH-  diff, 0, diff, HEIGHT, BLACK);
        DrawRectangle(diff, 0, WIDTH - diff * 2,border, BLACK);
        DrawRectangle(diff, HEIGHT - border, WIDTH - diff * 2, border, BLACK);

        DrawLineEx((Vector2){diff, border}, (Vector2){WIDTH - diff, border}, 2, outlineCol);
        DrawLineEx((Vector2){diff, border}, (Vector2){diff, HEIGHT - border}, 2, outlineCol);
        DrawLineEx((Vector2){diff, HEIGHT -border}, (Vector2){WIDTH - diff, HEIGHT - border}, 2, outlineCol);
        DrawLineEx((Vector2){WIDTH - diff, border}, (Vector2){WIDTH -diff, HEIGHT - border}, 2, outlineCol);

        EndTextureMode();
        int ws = WIDTH * RSCALE; 
        int hs = HEIGHT * RSCALE;
        Rectangle dest = (Rectangle){0, 0, ws, hs};

        //beginshadermode for the blit
        BeginShaderMode(postProcess_frag);
        DrawTexturePro(targetTex.texture, (Rectangle){0, 0, WIDTH, -HEIGHT}, dest, Vector2Zero(), 0, WHITE);
        EndShaderMode();

        
        // RenderWindow(&hello, &font);

        switch(currentScene){
            case Menu:
                MenuUpdate();
            break;
            case MapScene:
                MapUIRender();
                break;
            case Battle:
                BattleUIRender();
            break;
            case Editor:
               EditorUILoop();
            break;
        }

        EndDrawing();
    }

    UnloadShaders();
    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice(); 
    CloseWindow();
    return 0;
}
