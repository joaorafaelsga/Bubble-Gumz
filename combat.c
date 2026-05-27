#include "combat.h"

void UpdateHitbox(Hitbox *h)
{
    if (!h->active) return;

    h->timer += GetFrameTime();

    if (h->timer >= h->duration) {
        h->active = false;
    }
}

void DrawHitbox(Hitbox h)
{
    if (h.active)
        DrawRectangleLinesEx(h.rect, 2, RED);
}