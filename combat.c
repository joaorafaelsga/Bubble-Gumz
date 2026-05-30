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
    
    h->animTime += GetFrameTime();
    if (h->animTime >= 0.1f) {
        h->animTime = 0;
        h->frame = (h->frame + 1) % 2;
    }

    h->timer += GetFrameTime();

    if (h->timer >= h->duration)
        h->active = false;
}


void DrawHitbox(Hitbox h, int direction)
{
    if (!h.active) return;

    int frameH = h.texture.height / 4; 
    int frameW = h.texture.width;
    
    Rectangle source = { 0, h.frame * frameH, frameW, frameH };

    if (direction == -1) {
        source.x = frameW;
        source.width = -frameW;
    }

   
    Rectangle dest   = { h.rect.x, h.rect.y, h.rect.width, h.rect.height };
    DrawTexturePro(h.texture, source, dest, (Vector2){0,0}, 0, WHITE);
}