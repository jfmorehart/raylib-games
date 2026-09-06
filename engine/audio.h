#pragma once
// #include "raymath.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"
#include "raymath.h"
#include "shiploadouts.h"


typedef enum WaveType{
    Sine, 
    Saw, 
    Triangle,
    Square,
    WhiteNoise,
    PerlinNoise,
    FBMNoise
}WaveType;

typedef struct Note{
    //set reset
    bool active;
    float startTime;

    //set once
    float duration;
    float freq;
    float amp;
    WaveType wt;
    float phaseAdd;
    int harmonics;

    //gets set every buffer refill
    float phase;
}Note;

//wave types
typedef enum JMSound{
    Boom1, 
    Boom2, 
    Boom3, 
    Splash,
    Crack
}JMSound;
//stores the actual wave data, cannot switch types after alloc
typedef struct JMWaveBuffer{
    JMSound type;
    float * buffer;
    unsigned int bufferMax;
}JMWaveBuffer;

void PlayWave(JMSound type, float multiplier, float pan);
void PlayBulletSound(Gun g, Vector2 postion);
void PlayExplosionSound(float size, Vector2 position);
void PlaySplashSound(float size, Vector2 position);

AudioStream stream;
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096

#define MAX_NOTES 350

#define D 146.83
#define E 164
#define F 174
#define Fs 185.00
#define G 196
#define A 220
#define As 233

#define C2 261
#define D2 293

#define bpm 120

void InitAudio();

void EndAllNotes();

void ProcessAudio();