#include "player.h"

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed)
{
    Vector2 next = p->position;

    // CONTROLES
    if (p->controls == 0) {
        if (IsKeyDown(KEY_D)) { next.x += speed; p->direction = 1; }
        if (IsKeyDown(KEY_A)) { next.x -= speed; p->direction = -1; }
        if (IsKeyDown(KEY_W)) next.y -= speed;
        if (IsKeyDown(KEY_S)) next.y += speed;
    } else {
        if (IsKeyDown(KEY_RIGHT)) { next.x += speed; p->direction = 1; }
        if (IsKeyDown(KEY_LEFT)) { next.x -= speed; p->direction = -1; }
        if (IsKeyDown(KEY_UP)) next.y -= speed;
        if (IsKeyDown(KEY_DOWN)) next.y += speed;
    }

    int frameW = p->sprite.width / p->cols;
    int frameH = p->sprite.height / p->rows;

    Rectangle rectX = { next.x, p->position.y, frameW, frameH };
    Rectangle rectY = { p->position.x, next.y, frameW, frameH };
    

    bool colX = false;
    bool colY = false;

    for (int i = 0; i < wallCount; i++) {
        if (CheckCollisionRecs(rectX, walls[i])) colX = true;
        if (CheckCollisionRecs(rectY, walls[i])) colY = true;
    }

    if (!colX) p->position.x = next.x;
    if (!colY) p->position.y = next.y;

    // ANIMAÇÃO
    p->animTime += GetFrameTime();

    if (p->animTime >= animSpeed) {
        p->animTime = 0;
        p->frame++;

        if (p->frame >= p->totalFrames)
            p->frame = 0;
    }
    // KNOCKBACK
    if (p->isHit) {
        p->position.x += p->velocity.x * GetFrameTime();
        p->position.y += p->velocity.y * GetFrameTime();

        p->hitTimer += GetFrameTime();

        if (p->hitTimer > 0.2f) {
            p->isHit = false;
            p->velocity = (Vector2){0,0};
        }
    }
}

Rectangle GetPlayerRect(Player *p)
{
    int frameW = p->sprite.width / p->cols;
    int frameH = p->sprite.height / p->rows;

    return (Rectangle){
        p->position.x,
        p->position.y,
        frameW,
        frameH
    };
}