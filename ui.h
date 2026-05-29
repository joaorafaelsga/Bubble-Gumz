#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <stdlib.h>

// Dano com lista encadeada
typedef struct DamageNode {
    int damage;
    Vector2 position;
    float alpha;
    struct DamageNode *next;
} DamageNode;


void AddDamageText(DamageNode **head, int damage, Vector2 pos);
void UpdateAndDrawDamageTexts(DamageNode **head);
void FreeDamageTexts(DamageNode **head);

// Função da Barra de vida
void DrawHealthBar(int x, int y, int hp, int maxHp, Color color, const char* name);

#endif