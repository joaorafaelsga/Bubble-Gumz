#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <raylib.h>
#include "player.h"

//LISTA ENCADEADA
typedef struct GameSave {
    int id;
    int p1_hp;
    int p2_hp;
    Vector2 p1_pos;
    Vector2 p2_pos;
    struct GameSave *next; // ponteiro 
} GameSave;

extern char saveNames[10][30];

// Funções do sistema
void AddSaveState(GameSave **head, Player p1, Player p2);
void SaveToFile(GameSave *head);
GameSave* LoadFromFile(void);
bool LoadLatestSave(GameSave *head, Player *p1, Player *p2);
void FreeSaves(GameSave **head);

#endif