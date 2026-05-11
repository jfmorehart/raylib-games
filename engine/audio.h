#pragma once
// #include "raymath.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"
#include "raymath.h"


AudioStream stream;
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096
float buffer[BUFFER_SIZE];

float sineFrequency = 440;
float phase = 0;
float sineStartTime = 0;

extern float unscaledTime;

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

Note CreateNote(float duration, float freq, float amp, WaveType wt, int harmonics){
    Note n;
    n.active = true;
    n.duration = duration;
    n.freq = freq;
    n.amp = amp;
    n.wt = wt;
    n.phase = 0;
    n.phaseAdd = 2 * PI / (SAMPLE_RATE / n.freq);
    n.harmonics= harmonics;
    n.startTime = unscaledTime;
    return n;
}

int noteCount = 1;
#define MAX_NOTES 350
Note notes[MAX_NOTES];


Note song[350];
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

int songnum = 0;
float nstep;

void A2S(float duration, float freq, float amp, WaveType wt, int harmonics){
    if(songnum > MAX_NOTES) {
        TraceLog(LOG_WARNING, "not enough song slots\n");
    }
    song[songnum] = CreateNote(duration, freq, amp, wt, harmonics);
    songnum++;
}
void WriteSong(){
    float qtr = 60.00/ bpm;
    float eit = qtr / 2;
    float sixt = eit / 2;

    // //One measure rest
    // A2S(qtr * 1, D, 0, Sine, 1);
    // A2S(qtr * 1, D, 0, Sine, 1);
    // A2S(qtr * 1, D, 0, Sine, 1);

    //LEAD IN
    A2S(sixt, D, 0.3, Sine, 3);
    A2S(eit, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    //M1
    A2S(qtr + qtr + qtr, Fs, 0.3, Sine, 3);
        
    A2S(sixt, D, 0.3, Sine, 3);
    A2S(eit, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    //M2
    A2S(qtr + qtr + qtr, G, 0.3, Sine, 3);

    A2S(sixt, D, 0.3, Sine, 3);
    A2S(eit, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    A2S(qtr + qtr + qtr, As, 0.3, Sine, 3);

    A2S(sixt, A, 0.3, Sine, 3);
    A2S(sixt, D, 0, Sine, 3);
    A2S(sixt, As, 0.3, Sine, 3);
    A2S(sixt, A, 0.3, Sine, 3);

    A2S(qtr + qtr + qtr, G, 0.3, Sine, 3);

    A2S(sixt, D, 0.3, Sine, 3);
    A2S(sixt, D, 0.3, Sine, 3);
    A2S(sixt, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    A2S(qtr + qtr + qtr, Fs, 0.3, Sine, 3);

    A2S(sixt, D, 0.3, Sine, 3);
    A2S(eit, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    A2S(qtr + qtr + qtr, D2, 0.3, Sine, 3);

    A2S(sixt, D, 0.3, Sine, 3);
    A2S(eit, D, 0, Sine, 1);
    A2S(sixt, D, 0.3, Sine, 3);

    A2S(qtr + qtr + qtr, Fs, 0.3, Sine, 3);
    A2S(sixt, E, 0.3, Sine, 3);
    A2S(sixt, E, 0, Sine, 1);
    A2S(sixt, Fs, 0.3, Sine, 3);
    A2S(sixt, E, 0, Sine, 1);
    A2S(qtr + qtr + qtr, G, 0.3, Sine, 3);

    // END OF PART ONE

    // fall 1
    A2S(eit + sixt, D2, 0.3, Sine, 3); 
    A2S(sixt, C2, 0.3, Sine, 3); 
    A2S(sixt, As, 0.3, Sine, 3); 

    A2S(eit + sixt, D2, 0, Sine, 3); 

    A2S(eit + sixt, As, 0.3, Sine, 3); 
    A2S(sixt, A, 0.3, Sine, 3); 
    A2S(sixt, G, 0.3, Sine, 3); 

    A2S(eit + sixt, D2, 0, Sine, 3); 
    
    A2S(eit + sixt, A, 0.3, Sine, 3); 
    A2S(sixt, As, 0.3, Sine, 3); 
    A2S(sixt, A, 0.3, Sine, 3); 

    A2S(eit + sixt, 0, 0, Sine, 3);

    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(eit, D, 0, Sine, 3); 
    A2S(sixt * 0.5, D, 0.3, Sine, 3); 
    A2S(sixt * 0.5, D, 0, Sine, 3); 
    A2S(qtr, D, 0.3, Sine, 3); 

    //back up
    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(eit, D, 0, Sine, 3); 
    A2S(sixt * 0.5, D, 0.3, Sine, 3); 
    A2S(sixt * 0.5, D, 0, Sine, 3); 

    A2S(sixt, A, 0.3, Sine, 3); 
    A2S(eit + sixt, 0, 0, Sine, 3); 
    A2S(eit + sixt, A, 0.3, Sine, 3); 
    A2S(sixt, As, 0.3, Sine, 3); 
    A2S(sixt, C2, 0.3, Sine, 3); 

    A2S(eit + sixt, 0, 0, Sine, 3);  

    A2S(eit + sixt, As, 0.3, Sine, 3); 
    A2S(sixt, A, 0.3, Sine, 3); 
    A2S(sixt, As, 0.3, Sine, 3); 

    A2S(eit + sixt, 0, 0, Sine, 3);  
    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(eit, D, 0, Sine, 3); 
    A2S(sixt * 0.5, D, 0.3, Sine, 3); 
    A2S(sixt * 0.5, D, 0, Sine, 3); 
    A2S(qtr, D, 0.3, Sine, 3); 

    //3

    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(eit, D, 0, Sine, 3); 
    A2S(sixt * 0.5, D, 0.3, Sine, 3); 
    A2S(sixt * 0.5, D, 0, Sine, 3); 

    A2S(sixt, G, 0.3, Sine, 3); 
    A2S(eit + sixt, 0, 0, Sine, 3);  

    A2S(sixt + eit, G, 0.3, Sine, 3); 
    // A2S(eit, 0, 0, Sine, 3); 
    A2S(sixt * 0.5, A, 0.3, Sine, 3); 
    A2S(sixt * 0.5, 0, 0, Sine, 3); 

    A2S(sixt, As, 0.3, Sine, 3); 
    A2S(eit + sixt, 0, 0, Sine, 3);  

    A2S(sixt + eit, A, 0.3, Sine, 3); 
    // A2S(eit, 0, 0, Sine, 3); 
    A2S(sixt * 0.5, G, 0.3, Sine, 3); 
    A2S(sixt * 0.5, 0, 0, Sine, 3); 

    A2S(sixt, A, 0.3, Sine, 3); 
    A2S(eit + sixt, 0, 0, Sine, 3);  

    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(eit, D, 0, Sine, 3); 
    A2S(sixt * 0.5, D, 0.3, Sine, 3); 
    A2S(sixt * 0.5, D, 0, Sine, 3); 
    A2S(qtr, D, 0.3, Sine, 3); 

    //addams

    A2S(sixt, A, 0.3, Sine, 3); 
    A2S(eit, 0, 0, Sine, 3); 
    A2S(sixt * 0.5, G, 0.3, Sine, 3); 
    A2S(sixt * 0.5, 0, 0, Sine, 3); 

    A2S(sixt, Fs, 0.3, Sine, 3); 
    A2S(eit + sixt, 0, 0, Sine, 3);  

    A2S(sixt, D, 0.3, Sine, 3); 
    A2S(sixt + eit, D, 0, Sine, 3); 
    A2S(sixt + eit, D, 0.3, Sine, 3); 
    A2S(sixt, D, 0, Sine, 3); 

    A2S(sixt, E, 0.3, Sine, 3); 
    A2S(eit, 0, 0, Sine, 3); 
    A2S(sixt * 0.5, Fs, 0.3, Sine, 3); 
    A2S(sixt * 0.5, 0, 0, Sine, 3); 
    A2S(qtr + qtr + qtr, G, 0.3, Sine, 3); 
    // A2S(qtr, G, 0.3, Sine, 3); 
}


Note click[8];
void WriteClick(){
    float qtr = 60.00/ bpm;
    float eit = qtr / 2;
    float sixt = eit / 2;

    click[0] = CreateNote(sixt, D2, 0.1, Sine, 1);
    click[1] = CreateNote(eit + sixt, D2, 0, Sine, 1);
    click[2] = CreateNote(sixt, D2, 0.1, Sine, 1);
    click[3] = CreateNote(eit + sixt, D2, 0, Sine, 1);
    click[4] = CreateNote(sixt, D2, 0.1, Sine, 1);
    click[5] = CreateNote(eit + sixt, D2, 0, Sine, 1);
    click[6] = CreateNote(sixt, D2, 0.1, Sine, 1);
    click[7] = CreateNote(eit + sixt, D2, 0, Sine, 1);
}

int currentNoteInSong;
float nextNoteTime;

void PlaySong(float predictiveTime){
    if(predictiveTime- nextNoteTime < 0) return;

    if(currentNoteInSong >= songnum) currentNoteInSong = 0;
    printf("note: %d\n", currentNoteInSong);
    notes[noteCount] = song[currentNoteInSong];
    notes[noteCount].startTime = nextNoteTime;
    notes[noteCount].active = true;
    noteCount++;
    if(noteCount >= MAX_NOTES) noteCount = 0;

    notes[noteCount] = song[currentNoteInSong];
    notes[noteCount].wt = Saw;
    notes[noteCount].freq *= 0.5;
    notes[noteCount].harmonics= 1;
    notes[noteCount].startTime = nextNoteTime;
    notes[noteCount].active = true;
    notes[noteCount].amp *= 0.2;
    noteCount++;
    if(noteCount >= MAX_NOTES) noteCount = 0;
    
    notes[noteCount] = song[currentNoteInSong];
    notes[noteCount].wt = WhiteNoise;
    notes[noteCount].freq = nstep * 0.3;
    notes[noteCount].harmonics= 1;
    notes[noteCount].startTime = nextNoteTime;
    notes[noteCount].active = true;
    notes[noteCount].amp *= 0.02;
    noteCount++;
    if(noteCount >= MAX_NOTES) noteCount = 0;

    nextNoteTime += song[currentNoteInSong].duration;
    currentNoteInSong++;
}

int currentNoteInClick;
float nextClickTime;
void PlayClick(float predictiveTime){
   if(predictiveTime - nextClickTime < 0) return;

    if(currentNoteInClick >= 8) currentNoteInClick = 0;
    notes[noteCount] = click[currentNoteInClick];
    notes[noteCount].startTime = nextClickTime;
    notes[noteCount].active = true;
    nextClickTime += notes[noteCount].duration;
    noteCount++; 
    if(noteCount >= MAX_NOTES) noteCount = 0;
    currentNoteInClick++;
}


void InitAudio(){

    InitAudioDevice();

    // Set the number of samples the stream will keep in memory at a time to BUFFER_SIZE
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);

    // Init raw audio stream (sample rate: 44100, sample size: 32bit-float, channels: 1-mono)
    stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    float pan = 0.5;
    SetAudioStreamPan(stream, pan);
    PlayAudioStream(stream);
    // notes[0] = CreateNote(0, 440, 0.5, Saw, 1);;//, float freq, float amp, WaveType wt, int harmonics)
    // notes[1] = CreateNote(5, 587, 0.2, Sine, 3);
    // notes[2] = CreateNote(5, 659, 0.2, Sine, 3);
    noteCount = 0;
    WriteSong();
    WriteClick();


}


float CalcWave(WaveType wt, float phase, float nstep){//FOR USE WITH NOISE FUNCITONS, PASS NSTEP * FREQ AS THE PHASE PARAM
    
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
        return Perlin(nstep);
        case FBMNoise:
        return FBM(nstep, 2, 0.5, 6);
    }
    return 0;
}

float harm(WaveType wt, float phase, int harmonics, float dropoffexp, float nstep){
    float n = 0;
    for(int i =0; i < harmonics; i++){
        n += CalcWave(wt, phase * (i + 1), nstep) / pow(i + 1, dropoffexp);
    }
    return n;
}


bool started = false;
void ProcessAudio(){


    if(unscaledTime > 2){
        // PlaySong();
        if(!started){
            nextNoteTime = unscaledTime;
            nextClickTime = unscaledTime;
            started = true;
        }
        float endOfBufferTime = unscaledTime + (float)BUFFER_SIZE / SAMPLE_RATE;
        PlayClick(endOfBufferTime);
        PlaySong(endOfBufferTime);
    }

    if(IsKeyPressed(KEY_SPACE)){
        printf("playing notes\n");
        if(noteCount >= MAX_NOTES){noteCount = 0;}
        notes[noteCount] = CreateNote(1, 140, 0.3, Saw, 1);
        noteCount++;
        if(noteCount >= MAX_NOTES){noteCount = 0;}
        notes[noteCount] = CreateNote(1, 280, 0.5, Sine, 5);
        noteCount++;
    }

    if (IsAudioStreamProcessed(stream))
    {
        memset(buffer, 0, sizeof(buffer));
        float invs = 1.00 / SAMPLE_RATE;
        for(int n = 0; n < noteCount; n++){
            Note *note = &notes[n];
            if(!note->active) continue;
            float ntime = (unscaledTime - note->startTime )/ note->duration;
            float invd = 1.00 / note->duration;
            if(ntime > 1){
                note->active= false;
                continue;
            }

            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                nstep += 0.02; 
                
                // buffer[i] = CalcWave(FBMNoise, nstep * 0.3 + 0.5 * sin(nstep * 0.01));
                float ntimes = ((unscaledTime + i * invs) - note->startTime) * invd;
                if(ntimes < 0) continue;

                ntimes = Clamp(ntimes, 0, 1);//, float min, float max)
                float val = harm(note->wt, note->phase, note->harmonics, 1.5, nstep) * (1 - ntimes);
                // val += CalcWave(WhiteNoise, phase) * (1 - ntime);
      
                buffer[i] += val * note->amp;
                note->phase += note->phaseAdd;

                if (note->phase >= 2 * PI)
                {
                    note->phase -= 2 * PI;
                }
            }
        }

        //postprocessing
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            buffer[i] = tanh(buffer[i] * 1);
            // buffer[i]= Clamp(buffer[i], -1, 1);
        }
        UpdateAudioStream(stream, buffer, BUFFER_SIZE);
    }
}