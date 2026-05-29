#include "ui.h"

void AddDamageText(DamageNode **head, int damage, Vector2 pos) {
    // Alocação da memória
    DamageNode *newNode = (DamageNode *)malloc(sizeof(DamageNode));
    newNode->damage = damage;
    newNode->position = pos;
    newNode->alpha = 1.0f;
    newNode->next = *head;
    *head = newNode;
}

void UpdateAndDrawDamageTexts(DamageNode **head) {
    DamageNode *current = *head;
    DamageNode *prev = NULL;

    while (current != NULL) {
        current->position.y -= 40.0f * GetFrameTime();
        current->alpha -= 1.0f * GetFrameTime();

        if (current->alpha > 0.0f) {
            Color color = RED;
            color.a = (unsigned char)(current->alpha * 255);
            DrawText(TextFormat("-%d", current->damage), (int)current->position.x, (int)current->position.y, 20, color);
        }

        // tira da lista e liberta a memoria
        if (current->alpha <= 0.0f) {
            if (prev == NULL) *head = current->next;
            else prev->next = current->next;
            
            DamageNode *temp = current;
            current = current->next;
            free(temp); // Libertação da memória alocada dinamicamente
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void FreeDamageTexts(DamageNode **head) {
    DamageNode *current = *head;
    while (current != NULL) {
        DamageNode *temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}

void DrawHealthBar(int x, int y, int hp, int maxHp, Color color, const char* name) {
    int barWidth = 300;
    int barHeight = 20;
    float hpPercent = (float)hp / (float)maxHp;
    if (hpPercent < 0) hpPercent = 0;
    
    DrawRectangle(x, y, barWidth, barHeight, Fade(GRAY, 0.5f));
    DrawRectangle(x, y, (int)(barWidth * hpPercent), barHeight, color);
    DrawRectangleLines(x, y, barWidth, barHeight, BLACK);
    DrawText(name, x, y - 20, 20, BLACK);
}