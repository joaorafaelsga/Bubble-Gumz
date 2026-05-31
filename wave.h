#ifndef WAVE_H
#define WAVE_H

#include <stdbool.h>

#include "player.h"
#include "enemy.h"

typedef struct {
    int   currentWave;
    int   totalWaves;
    bool  waveCleared;
    float transitionTimer;
} WaveSystem;

void InitWaves(WaveSystem *ws, int totalWaves);

void SpawnWave(WaveSystem *ws, Enemy **enemies,Texture2D meleeSprite,Texture2D meleeAtk,
Texture2D rangedSprite, Texture2D rangedAtk, Texture2D bossSprite, Texture2D bossAtk, int fase);

void UpdateWaves(WaveSystem *ws,Enemy **enemies,EnemyBullet **bullets,Texture2D meleeSprite,Texture2D meleeAtk,
Texture2D rangedSprite,Texture2D rangedAtk, Texture2D bossSprite, Texture2D bossAtk, int fase, Player *p1, Player *p2);

void DrawWaveInfo(WaveSystem ws);
bool IsFaseComplete(WaveSystem ws);

#endif