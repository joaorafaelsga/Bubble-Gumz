#include <math.h> 
#include "cutscene.h"

void InitCutscene(Cutscene *c)
{
    c->images[0] = LoadTexture("assets/cutscenes/cutscene1.png");
    c->images[1] = LoadTexture("assets/cutscenes/cutscene2.png");
    c->images[2] = LoadTexture("assets/cenarios/Cenario2.png");
    //cena 1
    c->lines[0][0] = "Em uma tarde ensolarada, Bubble e Gumz";
    c->lines[0][1] = "descansavam no parque de skate.";
    c->lines[0][2] = "Silêncio, quase nenhum movimento.";
    c->lines[0][3] = "O lugar estava vazio demais... algo estava errado...";
    c->lineCount[0] = 4;
    //cena 2
    c->lines[1][0] = "Ate que...";
    c->lineCount[1] = 1;
    //cena 3
    c->lines[2][0] = "Ate que...";
    c->lineCount[2] = 1;    

    c->currentScene = 0;
    c->finished     = false;
    c->alpha        = 0.0f;
    c->fadingIn     = true;
}

void UpdateCutscene(Cutscene *c)
{
    if (c->finished) return;

    if (c->fadingIn) {
        c->alpha += GetFrameTime() * 2.0f;
        if (c->alpha >= 1.0f) {
            c->alpha    = 1.0f;
            c->fadingIn = false;
        }
        return;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        c->currentScene++;

        if (c->currentScene >= CUTSCENE_COUNT)
            c->finished = true;
        else {
            c->alpha    = 0.0f;
            c->fadingIn = true;
        }
    }
}

void DrawCutscene(Cutscene c)
{
    if (c.finished) return;

    int idx = c.currentScene;
    int nLines   = c.lineCount[idx];
    int fontSize = 20;
    int lineH    = 28;

    DrawTexturePro(c.images[idx],
        (Rectangle){0, 0, c.images[idx].width, c.images[idx].height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, 0, Fade(WHITE, c.alpha));

    // Faixa escurapara a legenda
    int faixaH = 30 + nLines * lineH + 30;
    DrawRectangle(0, GetScreenHeight() - faixaH,
                  GetScreenWidth(), faixaH,
                  Fade(BLACK, 0.75f * c.alpha));

    for (int i = 0; i < nLines; i++)
    {
        int tw = MeasureText(c.lines[idx][i], fontSize);
        DrawText(c.lines[idx][i],
                 (GetScreenWidth() - tw) / 2,
                 GetScreenHeight() - faixaH + 15 + i * lineH,
                 fontSize, Fade(WHITE, c.alpha));

    }
}

void FreeCutscene(Cutscene *c)
{
    for (int i = 0; i < CUTSCENE_COUNT; i++)
        UnloadTexture(c->images[i]);
}