#ifndef COMBAT_H
#define COMBAT_H

#include <raylib.h>

typedef struct Player Player; 

typedef struct {
    Rectangle rect;
    float duration;
    float timer;

    int damage;
    Vector2 knockback;

    bool active;

    Player *owner;

    Texture2D texture;
    int frame;
    float animTime;
} Hitbox;

void UpdateHitbox(Hitbox *h);
void DrawHitbox(Hitbox h, int direction);
void CheckHit(Hitbox *h, Player *target);

#endif