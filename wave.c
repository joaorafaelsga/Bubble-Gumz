#include "wave.h"
#include <stdio.h>

void InitWaves(WaveSystem *ws, int totalWaves)
{
    ws->currentWave     = 0;
    ws->totalWaves      = totalWaves;
    ws->waveCleared     = true;
    ws->transitionTimer = 0;
}

// Posições de spawn por wave e fase(com matriz)
static Vector2 spawnPosFase1[2][4] = {
    // Wave 1: 4 melee
    {{700,300},{650,350},{720,280},{680,320}},
    // Wave 2: 4 ranged
    {{750,300},{700,280},{760,340},{730,310}},
};

static Vector2 spawnPosFase2[1][6] = {
    // Wave 1: 3 melee + 3 ranged
    {{700,300},{650,350},{720,280},{680,320},{760,340},{730,310}},
};

void SpawnWave(WaveSystem *ws, Enemy **enemies,
               Texture2D meleeSprite, Texture2D meleeAtk,
               Texture2D rangedSprite, Texture2D rangedAtk,
               int fase)
{
    FreeEnemies(enemies);

    if (fase == 1)
    {
        int w = ws->currentWave;
        if (w == 0) {
            // Wave 1: 4 melee
            for (int i = 0; i < 4; i++) {
                Enemy *e = CreateEnemy(spawnPosFase1[0][i],
                                       ENEMY_MELEE, meleeSprite, meleeAtk);
                e->next  = *enemies;
                *enemies = e;
            }
        } else {
            // Wave 2: 4 ranged
            for (int i = 0; i < 4; i++) {
                Enemy *e = CreateEnemy(spawnPosFase1[1][i],
                                       ENEMY_RANGED, rangedSprite, rangedAtk);
                e->next  = *enemies;
                *enemies = e;
            }
        }
    }
    else // fase == 2
    {
        // Wave única: 3 melee + 3 ranged
        for (int i = 0; i < 3; i++) {
            Enemy *e = CreateEnemy(spawnPosFase2[0][i],
                                   ENEMY_MELEE, meleeSprite, meleeAtk);
            e->next  = *enemies;
            *enemies = e;
        }
        for (int i = 3; i < 6; i++) {
            Enemy *e = CreateEnemy(spawnPosFase2[0][i],
                                   ENEMY_RANGED, rangedSprite, rangedAtk);
            e->next  = *enemies;
            *enemies = e;
        }
    }

    ws->waveCleared = false;
}

void UpdateWaves(WaveSystem *ws, Enemy **enemies,
                 EnemyBullet **bullets,
                 Texture2D meleeSprite, Texture2D meleeAtk,
                 Texture2D rangedSprite, Texture2D rangedAtk,
                 int fase)
{
    if (ws->waveCleared)
    {
        ws->transitionTimer += GetFrameTime();

        if (ws->transitionTimer >= 2.0f) // 2s de pausa entre waves
        {
            ws->transitionTimer = 0;
            ws->currentWave++;

            if (ws->currentWave < ws->totalWaves) {
                SpawnWave(ws, enemies,
                          meleeSprite, meleeAtk,
                          rangedSprite, rangedAtk, fase);
            }
        }
        return;
    }

    // Checa se a wave foi limpa
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