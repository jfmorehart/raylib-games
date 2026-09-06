#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"
#include "raymath.h"
#include "audio.h"


#define RFXGEN_IMPLEMENTATION 
#include"vendor/rfxgen/rfxgen.h"

AudioStream stream;
float buffer[BUFFER_SIZE * 2];

float sineFrequency = 440;
float phase = 0;
float sineStartTime = 0;

extern float unscaledTime;

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
Note notes[MAX_NOTES];

Note song[350];
int songnum = 0;
float nstep;

Note song2[350];

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

//stores original (generated) sounds. large files
JMWaveBuffer wave_buffers [5];

//stores the instance, pooled, can be messed with safely.
typedef struct JMWaveInstance{
    bool active;
    JMSound type;
    float pan;// rigtht to left, 0 to 1
    float multiplier;
    unsigned int bufferPoint;
}JMWaveInstance;

int wave_next;
#define MAX_WAVES 30
JMWaveInstance wave_instances[MAX_WAVES];

void PlayWave(JMSound type, float multiplier, float pan){
    wave_instances[wave_next].type = type;
    wave_instances[wave_next].pan = pan;
    wave_instances[wave_next].multiplier = multiplier;
    wave_instances[wave_next].active = true;
    wave_instances[wave_next].bufferPoint = 0;
    wave_next++;
    if(wave_next >= MAX_WAVES) wave_next = 0;
}

float WPos2Pan(Vector2 worldPos){
    float x = WorldToScreen(worldPos).x;
    return (x / WIDTH);
}
int last;
void PlayBulletSound(Gun g, Vector2 postion){
    //todo robustify!
    PlayWave((JMSound)last, 0.1, WPos2Pan(postion));

    last++;
    if(last > 2) last = 0;
}
void PlayExplosionSound(float size, Vector2 position){
    PlayWave(Crack, 0.03 * size, WPos2Pan(position));
}
void PlaySplashSound(float size, Vector2 position){
    PlayWave(Splash, 0.01 * size, WPos2Pan(position));
}

void InitAudio(){

    InitAudioDevice();

    // Set the number of samples the stream will keep in memory at a time to BUFFER_SIZE
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);

    // Init raw audio stream (sample rate: 44100, sample size: 32bit-float, channels: 2-stereo)
    stream = LoadAudioStream(SAMPLE_RATE, 32, 2);

    float pan = 0.5;
    SetAudioStreamPan(stream, pan);
    PlayAudioStream(stream);

    noteCount = 0;
    WriteSong();
    WriteClick();
    


    unsigned int boom_frames;
    WaveParams boom;
    boom = LoadWaveParams("assets/boom.rfx");
    wave_buffers[0].type = Boom1;
    wave_buffers[0].buffer = GenerateWave(boom, &boom_frames);
    wave_buffers[0].bufferMax =  boom_frames;

    boom = LoadWaveParams("assets/boom2.rfx");
    wave_buffers[1].type = Boom2;
    wave_buffers[1].buffer = GenerateWave(boom, &boom_frames);
    wave_buffers[1].bufferMax =  boom_frames;

    boom = LoadWaveParams("assets/boom3.rfx");
    wave_buffers[2].type = Boom3;
    wave_buffers[2].buffer = GenerateWave(boom, &boom_frames);
    wave_buffers[2].bufferMax =  boom_frames;

    boom = LoadWaveParams("assets/splash.rfx");
    wave_buffers[3].type = Splash;
    wave_buffers[3].buffer = GenerateWave(boom, &boom_frames);
    wave_buffers[3].bufferMax =  boom_frames;

    boom = LoadWaveParams("assets/crack.rfx");
    wave_buffers[4].type = Crack;
    wave_buffers[4].buffer = GenerateWave(boom, &boom_frames);
    wave_buffers[4].bufferMax =  boom_frames;

}

void EndAllNotes(){
    for(int i =0; i < noteCount; i++){
        if(notes[i].active){
            notes[i].duration = fmin(notes[i].duration, (unscaledTime - notes[i].startTime) + 1);
        }
    }
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

void OnSwitchScene(){
    EndAllNotes();
}

bool started = false;
Note * noiseNote;
Note *crashNote;



void ProcessAudio(){
    
    // if(IsKeyPressed(KEY_ONE)){
    //     PlayWave(Boom1, 0.2);
    // }
    // if(IsKeyPressed(KEY_TWO)){
    //     PlayWave(Boom2, 0.2);
    // }
    // if(IsKeyPressed(KEY_THREE)){
    //     PlayWave(Boom3, 0.2);
    // }
    // if(IsKeyPressed(KEY_FOUR)){
    //     PlayWave(Splash, 0.2);
    // }
    // if(IsKeyPressed(KEY_FIVE)){
    //     PlayWave(Crack, 0.2);
    // }

    switch(currentScene){
        case Menu:
            if(unscaledTime > 2){
                if(!started){
                    nextNoteTime = unscaledTime;
                    nextClickTime = unscaledTime;
                    started = true;
                }
            float endOfBufferTime = unscaledTime + (float)BUFFER_SIZE / SAMPLE_RATE;
            PlayClick(endOfBufferTime);
            PlaySong(endOfBufferTime);
        }   
        break;
        case Battle:
            if(noiseNote){
                if(noiseNote->duration > 9998) {
                    //hot loop
                    break;
                }
            }
            //once on startup
            notes[noteCount] = CreateNote(9999, 1, 0.05, PerlinNoise, 1);
            noiseNote = &notes[noteCount];
            noteCount++;
            if(noteCount >= MAX_NOTES) noteCount = 0;
            notes[noteCount] = CreateNote(99999, 1, 0.05, FBMNoise, 1);
            crashNote = &notes[noteCount];
            noteCount++;
            if(noteCount >= MAX_NOTES) noteCount = 0;
        break;
    }
    
    if (IsAudioStreamProcessed(stream))
    {
        memset(buffer, 0, sizeof(buffer));
        
        float invs = 1.00 / SAMPLE_RATE;

        //music section
        float nstepStart = nstep;
        for(int n = 0; n < noteCount; n++){
            
            Note *note = &notes[n];
            if(!note->active) continue;
            float ntime = (unscaledTime - note->startTime )/ note->duration;
            float invd = 1.00 / note->duration;
            if(ntime > 1){
                note->active= false;
                continue;
            }
            nstep = nstepStart;
            for (int f = 0; f < BUFFER_SIZE; f++)
            {
                int i = f * 2; //each F frame has two I's, left and right

                nstep += 0.02; 
                
                float ntimes = ((unscaledTime + f * invs) - note->startTime) * invd;
                if(ntimes < 0) continue;

                if(note == crashNote){
                    note->amp = (sin(unscaledTime + f * invs) + 1) * 0.5 * 0.1;
                }

                ntimes = Clamp(ntimes, 0, 1);//, float min, float max)
                float val = harm(note->wt, note->phase, note->harmonics, 1.5, nstep) * (1 - ntimes);
      
                //LEFT
                buffer[i] += val * note->amp * 1;
                
                //RIGHT
                buffer[i + 1] += val * note->amp * 1; 


                note->phase += note->phaseAdd;

                if (note->phase >= 2 * PI)
                {
                    note->phase -= 2 * PI;
                }
            }
        }

        for (int f = 0; f < BUFFER_SIZE; f++)
        {      
            int i = f * 2;
            for(int w = 0; w < MAX_WAVES; w++){
                JMWaveInstance * thisWave;
                thisWave = &wave_instances[w];
                
                JMWaveBuffer * soundOriginal = &wave_buffers[thisWave->type];

                if(thisWave->active){
                    //LEFT
                    buffer[i] += soundOriginal->buffer[thisWave->bufferPoint] * thisWave->multiplier *  (1 - thisWave->pan);

                    //RIGHT
                    buffer[i + 1] += soundOriginal->buffer[thisWave->bufferPoint] * thisWave->multiplier * (thisWave->pan);
                    thisWave->bufferPoint++;
                    if(thisWave->bufferPoint >= soundOriginal->bufferMax){
                        thisWave->active = false;
                    }
                }
            }
        }


        //postprocessing
        for (int i = 0; i < BUFFER_SIZE * 2; i++)
        {
            buffer[i] = tanh(buffer[i] * 1);
            // buffer[i]= Clamp(buffer[i], -1, 1);
        }
        UpdateAudioStream(stream, buffer, BUFFER_SIZE);
    }
}