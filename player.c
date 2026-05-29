#include "player.h"

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed)
{
    Vector2 next = p->position;
    bool moving = false;

    if (p->controls == 0) {
        if (IsKeyDown(KEY_D)) { next.x += speed; p->direction = 1; moving = true; }
        if (IsKeyDown(KEY_A)) { next.x -= speed; p->direction = -1; moving = true; }
        if (IsKeyDown(KEY_W)) { next.y -= speed; moving = true; }
        if (IsKeyDown(KEY_S)) { next.y += speed; moving = true; }
    } else {
        if (IsKeyDown(KEY_RIGHT)) { next.x += speed; p->direction = 1; moving = true; }
        if (IsKeyDown(KEY_LEFT)) { next.x -= speed; p->direction = -1; moving = true; }
        if (IsKeyDown(KEY_UP)) { next.y -= speed; moving = true; }
        if (IsKeyDown(KEY_DOWN)) { next.y += speed; moving = true; }
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

    if (p->state != STATE_ATTACK)
    {
        p->state = moving ? STATE_WALK : STATE_SPRITE;
    }

    // ANIMAÇÃO 
    p->animTime += GetFrameTime();

    if (p->animTime >= animSpeed)
    {
        p->animTime = 0;
        p->frame++;

        if (p->frame >= p->totalFrames)
            p->frame = 0;
    }

    if (p->state == STATE_ATTACK)
    {
        p->stateTimer += GetFrameTime();

        if (p->stateTimer > 0.3f)
        {
            p->state = STATE_SPRITE;
            p->stateTimer = 0;
            p->frame = 0;
            p->animTime = 0;
        }
    }

    if (p->isHit)
    {
        p->position.x += p->velocity.x * GetFrameTime();
        p->position.y += p->velocity.y * GetFrameTime();

        p->hitTimer += GetFrameTime();

        if (p->hitTimer > 0.2f)
        {
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