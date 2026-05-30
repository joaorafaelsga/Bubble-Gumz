#include "projectile.h"

void UpdateProjectile(Projectile *p)
{
    if (!p->active) return;

    p->rect.x += p->velocity.x * GetFrameTime();
    p->rect.y += p->velocity.y * GetFrameTime();

    p->animTime += GetFrameTime();
    if (p->animTime >= 0.1f) {
        p->animTime = 0;
        p->frame = (p->frame + 1) % 2;
    }

    p->timer += GetFrameTime();
    if (p->timer >= p->lifetime)
        p->active = false;
}

void DrawProjectile(Projectile p, int direction)
{
    if (!p.active) return;

    int frameH = p.texture.height / 4; // 4 linhas no sheet
    int frameW = p.texture.width;

    Rectangle source = { 0, (2 + p.frame) * frameH, frameW, frameH };
    if (direction == -1) {
        source.x     = frameW;
        source.width = -frameW;
    }
    Rectangle dest   = { p.rect.x, p.rect.y, p.rect.width, p.rect.height };
    DrawTexturePro(p.texture, source, dest, (Vector2){0,0}, 0, WHITE);
}