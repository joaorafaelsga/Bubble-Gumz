#ifndef COMBAT_H
#define COMBAT_H

#include <raylib.h>

typedef struct {
    Rectangle rect;
    float duration;
    float timer;

    int damage;
    Vector2 knockback;

    bool active;
} Hitbox;

void UpdateHitbox(Hitbox *h);
void DrawHitbox(Hitbox h);

#endif