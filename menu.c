#include "menu.h"
#include <stdio.h>
#include <string.h>

static const char *menuLabels[MENU_COUNT] = {
    "PLAY",
    "NEW GAME",
    "SAVES",
    "SCORES",
    "SAIR"
};

// SAVE
void SaveGame(int fase)
{
    FILE *f = fopen("save.dat", "w");
    if (f) {
        fprintf(f, "%d\n", fase);
        fclose(f);
    }
}

int LoadGame()
{
    FILE *f = fopen("save.dat", "r");
    if (!f) return -1;

    int fase = -1;
    fscanf(f, "%d", &fase);
    fclose(f);
    return fase;
}

void DeleteSave()
{
    remove("save.dat");
}


// MENU
void InitMenu(Menu *m, Texture2D bg)
{
    m->currentItem = 0;
    m->background  = bg;
}

GameScreen UpdateMenu(Menu *m)
{
    if (IsKeyPressed(KEY_DOWN))
        m->currentItem = (m->currentItem + 1) % MENU_COUNT;

    if (IsKeyPressed(KEY_UP))
        m->currentItem = (m->currentItem - 1 + MENU_COUNT) % MENU_COUNT;

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (m->currentItem)
        {
            case MENU_PLAY:
                return SCREEN_GAME;

            case MENU_NEWGAME:
                DeleteSave();
                // scores são resetados no main ao chamar essa opção
                return SCREEN_MENU;

            case MENU_SAVES:
            case MENU_SCORES:
                // exibidos no DrawMenu, sem troca de tela
                return SCREEN_MENU;

            case MENU_QUIT:
                CloseWindow();
                break;
        }
    }

    return SCREEN_MENU;
}

void DrawMenu(Menu m, int topScore, const char *topName, int savedFase)
{
    // Fundo
    DrawTexturePro(m.background,
        (Rectangle){0, 0, m.background.width, m.background.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, 0, WHITE);

    // Título
    DrawText("BUBBLE-GUMZ", 240, 80, 48, YELLOW);

    // Itens do menu
    int startY = 220;
    for (int i = 0; i < MENU_COUNT; i++)
    {
        Color cor = (i == m.currentItem) ? YELLOW : WHITE;
        int   tam = (i == m.currentItem) ? 32 : 24;
        DrawText(menuLabels[i], 340, startY + i * 55, tam, cor);
    }

    // Painel de SAVES
    if (m.currentItem == MENU_SAVES)
    {
        DrawRectangle(520, 200, 220, 80, Fade(BLACK, 0.7f));
        if (savedFase >= 0)
            DrawText(TextFormat("Fase salva: %d", savedFase), 535, 225, 20, GREEN);
        else
            DrawText("Sem save", 535, 225, 20, GRAY);
    }

    // Painel de SCORES
    if (m.currentItem == MENU_SCORES)
    {
        DrawRectangle(520, 200, 220, 80, Fade(BLACK, 0.7f));
        DrawText("TOP SCORE:", 535, 210, 18, WHITE);
        DrawText(TextFormat("%s  %d pts", topName, topScore), 535, 235, 18, YELLOW);
    }

    // Instrução
    DrawText("SETAS + ENTER", 310, 530, 18, GRAY);
}