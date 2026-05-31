#ifndef MENU_H
#define MENU_H

#include <raylib.h>
#include <stdbool.h>

typedef enum {
    MENU_PLAY,
    MENU_NEWGAME,
    MENU_SAVES,
    MENU_SCORES,
    MENU_QUIT,
    MENU_COUNT
} MenuItem;

typedef enum {
    SCREEN_MENU,
    SCREEN_GAME,
    SCREEN_CUTSCENE
} GameScreen;

typedef struct {
    int currentItem;
    Texture2D background;
} Menu;

// Save
void SaveGame(int fase);
int  LoadGame(); 
void DeleteSave();

void InitMenu(Menu *m, Texture2D bg);
GameScreen UpdateMenu(Menu *m);
void DrawMenu(Menu m, int topScore, const char *topName, int savedFase);

#endif