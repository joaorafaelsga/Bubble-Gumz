#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <raylib.h>
#include <stdbool.h>

#define CUTSCENE_COUNT 4
#define MAX_LINES 6

typedef struct {
    Texture2D images[CUTSCENE_COUNT];
    const char *lines[CUTSCENE_COUNT][MAX_LINES];
    int lineCount[CUTSCENE_COUNT]; 
    const char *texts[CUTSCENE_COUNT];
    int   currentScene;
    bool  finished;

    // Efeito de fade
    float alpha;
    bool  fadingIn;
} Cutscene;

void InitCutscene(Cutscene *c);
void UpdateCutscene(Cutscene *c);
void DrawCutscene(Cutscene c);
void FreeCutscene(Cutscene *c);

#endif