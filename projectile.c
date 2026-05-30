#include "projectile.h"

void UpdateProjectile(Projectile *p)
{
    if (!p->active) return;

    p->rect.x += p->velocity.x * GetFrameTime();
    p->rect.y += p->velocity.y * GetFrameTime();

    p->timer += GetFrameTime();

    if (p->timer >= p->lifetime)
        p->active = false;
}

void DrawProjectile(Projectile p)
{
    if (p.active)
        DrawRectangleRec(p.rect, PINK);
}