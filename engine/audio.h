#pragma once
// #include "raymath.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include "helpers.h"


AudioStream stream;
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
float buffer[BUFFER_SIZE];

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
typedef struct Note{
    //set once
    float startTime;
    float duration;
    float freq;
    float amp;
    WaveType wt;
    float phaseAdd;
    float phase;
}Note;

int noteCount = 3;
Note notes[3];

void InitAudio(){

    InitAudioDevice();

    // Set the number of samples the stream will keep in memory at a time to BUFFER_SIZE
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);

    // Init raw audio stream (sample rate: 44100, sample size: 32bit-float, channels: 1-mono)
    stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    float pan = 0.5;
    SetAudioStreamPan(stream, pan);
    PlayAudioStream(stream);
     for(int n = 0; n < noteCount; n++){
        notes[n].wt = Sine;
        notes[n].amp = 1;//R01();
        notes[n].freq = (R01() * 220) * 1.05946309436 * 2 * n;
        notes[n].phase = 0;
        notes[n].duration = 1;
        notes[n].phaseAdd = 2 * PI / (SAMPLE_RATE / notes[n].freq);
    }
}


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
    return 0;
}

float harm(float phase, int harmonics, float dropoffexp){
    float n = 0;
    for(int i =0; i < harmonics; i++){
        n += CalcWave(Sine, phase * (i + 1)) / pow(i + 1, dropoffexp);
    }
    return n;
}

void ProcessAudio(){

    if (IsAudioStreamProcessed(stream))
    {
        for(int n = 0; n < noteCount; n++){
            Note *note = &notes[n];
            // note->freq = (GetCharPressed() / 130.00) * 440;
            // printf("freq %f\n",  note->freq);
            // note->phaseAdd =  2 * PI / (SAMPLE_RATE / note->freq);

            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                nstep += 0.02; 
                
                // buffer[i] = CalcWave(FBMNoise, nstep * 0.3 + 0.5 * sin(nstep * 0.01));
                float val = fmaxf(harm(note->phase, 10, 2), CalcWave(PerlinNoise, nstep * 0.4) * 0.4); 
                if(n == 0){
                    buffer[i] = val * note->amp / noteCount;
                }else{
                    buffer[i] += val * note->amp / noteCount;
                }

                note->phase += note->phaseAdd;

                if (note->phase >= 2 * PI)
                {
                    note->phase -= 2 * PI;
                }
            }
        }

        UpdateAudioStream(stream, buffer, BUFFER_SIZE);
    }
}