#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <stdlib.h>

// Lista Encadeada para os textos de dano flutuantes
typedef struct DamageNode {
    int damage;
    Vector2 position;
    float alpha;
    struct DamageNode *next;
} DamageNode;

// Funções da Lista Encadeada e Alocação Dinâmica
void AddDamageText(DamageNode **head, int damage, Vector2 pos);
void UpdateAndDrawDamageTexts(DamageNode **head);
void FreeDamageTexts(DamageNode **head);

// Função da Barra de vida
void DrawHealthBar(int x, int y, int hp, int maxHp, Color color, const char* name);

#endif