#include "combat.h"
#include "player.h" 

void CheckHit(Hitbox *h, Player *target)
{
    if (!h->active) return;
    if (h->owner == target) return;

    if (CheckCollisionRecs(h->rect, GetPlayerRect(target)))
    {   
        float attackDir = h->owner->direction;
        bool attackFromFront = (target->direction == -attackDir);

        if (target->isBlocking && attackFromFront)
        {
            target->velocity.x = h->knockback.x * 0.2f;
            target->velocity.y = h->knockback.y * 0.2f;
            h->active = false;
            return;
        }

        target->isHit = true;
        target->hitTimer = 0;
        target->velocity = h->knockback;
        h->active = false;
    }
}
void UpdateHitbox(Hitbox *h)
{
    if (!h->active) return;

    h->timer += GetFrameTime();

    if (h->timer >= h->duration)
        h->active = false;
}


void DrawHitbox(Hitbox h)
{
    if (h.active)
        DrawRectangleLinesEx(h.rect, 2, RED);
}