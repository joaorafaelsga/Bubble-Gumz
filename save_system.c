#include "save_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char saveNames[10][30]; 

void AddSaveState(GameSave **head, Player p1, Player p2) {
    GameSave *newSave = (GameSave *)malloc(sizeof(GameSave));
    
    int nextId = (*head == NULL) ? 1 : (*head)->id + 1;
    
    newSave->id = nextId;
    newSave->p1_hp = p1.hp;
    newSave->p2_hp = p2.hp;
    newSave->p1_pos = p1.position;
    newSave->p2_pos = p2.position;
    
    
    newSave->next = *head;
    *head = newSave;
    
    // Atualiza a MATRIZ com um texto de identificação do save
    snprintf(saveNames[(newSave->id - 1) % 10], 30, "Save State %d", newSave->id);
}

void SaveToFile(GameSave *head) {
    FILE *file = fopen("savegame.txt", "w");
    if (file == NULL) return;
    
    GameSave *current = head;
    while (current != NULL) {
        // Salva as informações da lista no txt
        fprintf(file, "%d %d %d %f %f %f %f\n", 
            current->id, 
            current->p1_hp, current->p2_hp, 
            current->p1_pos.x, current->p1_pos.y,
            current->p2_pos.x, current->p2_pos.y);
        current = current->next;
    }
    fclose(file);
}

GameSave* LoadFromFile(void) {
    FILE *file = fopen("savegame.txt", "r");
    if (file == NULL) return NULL;
    
    GameSave *head = NULL;
    GameSave *tail = NULL;
    
    int id, p1hp, p2hp;
    float p1x, p1y, p2x, p2y;
    
    while (fscanf(file, "%d %d %d %f %f %f %f", &id, &p1hp, &p2hp, &p1x, &p1y, &p2x, &p2y) == 7) {
        GameSave *newSave = (GameSave *)malloc(sizeof(GameSave));
        newSave->id = id;
        newSave->p1_hp = p1hp;
        newSave->p2_hp = p2hp;
        newSave->p1_pos = (Vector2){p1x, p1y};
        newSave->p2_pos = (Vector2){p2x, p2y};
        newSave->next = NULL;
        
        // salva os nomes com os dados lidos
        snprintf(saveNames[(id - 1) % 10], 30, "Save State %d", id);
        
        if (head == NULL) {
            head = newSave;
            tail = newSave;
        } else {
            tail->next = newSave;
            tail = newSave;
        }
    }
    fclose(file);
    return head;
}

// APLICA O SAVE MAIS RECENTE AOS JOGADORES
bool LoadLatestSave(GameSave *head, Player *p1, Player *p2) {
    if (head == NULL) return false;
    
    p1->hp = head->p1_hp;
    p2->hp = head->p2_hp;
    p1->position = head->p1_pos;
    p2->position = head->p2_pos;
    
    // Reseta status para evitar bugs nas animações
    p1->state = 0; p2->state = 0;
    p1->isHit = false; p2->isHit = false;
    
    return true;
}

// Evitar Memory Leak
void FreeSaves(GameSave **head) {
    GameSave *current = *head;
    while (current != NULL) {
        GameSave *temp = current;
        current = current->next;
        free(temp); // Libera a memória 
    }
    *head = NULL;
}