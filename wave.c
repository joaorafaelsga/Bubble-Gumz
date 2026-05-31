#include "wave.h"
#include <stdio.h>

void InitWaves(WaveSystem *ws, int totalWaves)
{
    ws->currentWave = 0;
    ws->totalWaves = totalWaves;
    ws->waveCleared = true;
    ws->transitionTimer = 0;
}

static Vector2 spawnPosFase1[2][4] = {
    {{700,300},{650,350},{720,280},{680,320}}, // Wave 1 da Fase 1
    {{750,300},{700,280},{760,340},{730,310}}, // Wave 2 da Fase 1
};

static Vector2 spawnPosFase2[2][6] = {
    {{700,300},{650,350},{720,280},{680,320},{760,340},{730,310}}, // Wave 1 da Fase 2
    {{700,320},{0,0},{0,0},{0,0},{0,0},{0,0}}                      // Wave 2 da Fase 2 (Boss)
};

void SpawnWave(WaveSystem *ws, Enemy **enemies,
               Texture2D meleeSprite, Texture2D meleeAtk,
               Texture2D rangedSprite, Texture2D rangedAtk,
               Texture2D bossSprite, Texture2D bossAtk,
               int fase)
{
    FreeEnemies(enemies);

    if (fase == 1)
    {
        int w = ws->currentWave;
        if (w == 0) {
            for (int i = 0; i < 4; i++) {
                Enemy *e = CreateEnemy(spawnPosFase1[0][i],ENEMY_MELEE, meleeSprite, meleeAtk);
                e->next  = *enemies;
                *enemies = e;
            }
        } else {
            for (int i = 0; i < 4; i++) {
                Enemy *e = CreateEnemy(spawnPosFase1[1][i],ENEMY_RANGED, rangedSprite, rangedAtk);
                e->next  = *enemies;
                *enemies = e;
            }
        }
    }
    else 
    {
        int w = ws->currentWave;
        if (w == 0) {
            for (int i = 0; i < 3; i++) {
                Enemy *e = CreateEnemy(spawnPosFase2[0][i], ENEMY_MELEE, meleeSprite, meleeAtk);
                e->next  = *enemies;
                *enemies = e;
            }
            for (int i = 3; i < 6; i++) {
                Enemy *e = CreateEnemy(spawnPosFase2[0][i],  ENEMY_RANGED, rangedSprite, rangedAtk);
                e->next  = *enemies;
                *enemies = e;
            }
        } else {
            // BATALHA CONTRA O BOSS TRATOR!
            Enemy *boss = CreateEnemy(spawnPosFase2[1][0], ENEMY_BOSS, bossSprite, bossAtk);
            boss->next = *enemies;
            *enemies = boss;
        }
    }

    ws->waveCleared = false;
}

void UpdateWaves(WaveSystem *ws,Enemy **enemies,EnemyBullet **bullets,Texture2D meleeSprite, 
Texture2D meleeAtk,Texture2D rangedSprite,Texture2D rangedAtk, Texture2D bossSprite, Texture2D bossAtk, int fase, Player *p1, Player *p2)
{
    if (ws->waveCleared)
    {
        ws->transitionTimer += GetFrameTime();

        if (ws->transitionTimer >= 2.0f) 
        {
            ws->transitionTimer = 0;
            ws->currentWave++;

            if (ws->currentWave < ws->totalWaves) {
                SpawnWave(ws, enemies,meleeSprite,meleeAtk,rangedSprite,rangedAtk,bossSprite,bossAtk,fase);
                if (p1->isDead) {
                    p1->isDead = false;
                    p1->hp     = p1->maxHp / 2;
                    p1->state  = STATE_SPRITE;
                }
                if (p2->isDead) {
                    p2->isDead = false;
                    p2->hp     = p2->maxHp / 2;
                    p2->state  = STATE_SPRITE;
                }
            }
        }
        return;
    }

    if (CountActiveEnemies(*enemies) == 0)
        ws->waveCleared = true;
}

void DrawWaveInfo(WaveSystem ws)
{
    if (ws.waveCleared && ws.currentWave < ws.totalWaves)
        DrawText("WAVE COMPLETA!", 300, 280, 28, YELLOW);

    DrawText(TextFormat("Wave: %d / %d", ws.currentWave + 1, ws.totalWaves),
             10, 560, 20, WHITE);
}

bool IsFaseComplete(WaveSystem ws)
{
    return ws.currentWave >= ws.totalWaves && ws.waveCleared;
}