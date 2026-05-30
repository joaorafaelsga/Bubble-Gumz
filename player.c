#include "player.h"

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed)
{
    Vector2 next = p->position;
    bool moving = false;

    if (p->controls == 0) {
        p->isBlocking = (IsKeyDown(KEY_Q) && p->state != STATE_ATTACK);

        if (!p->isBlocking) { 
            if (IsKeyDown(KEY_D)) { next.x += speed; p->direction =  1; moving = true; }
            if (IsKeyDown(KEY_A)) { next.x -= speed; p->direction = -1; moving = true; }
            if (IsKeyDown(KEY_W)) { next.y -= speed; moving = true; }
            if (IsKeyDown(KEY_S)) { next.y += speed; moving = true; }
        }

        if (IsKeyPressed(KEY_R) && p->state != STATE_ATTACK && !p->isBlocking) {
            p->state    = STATE_ATTACK;
            p->frame    = 0;
            p->animTime = 0;
        }
        
    } 
    else {
        p->isBlocking = (IsKeyDown(KEY_KP_0) && p->state != STATE_ATTACK);

        if (!p->isBlocking) {
            if (IsKeyDown(KEY_RIGHT)) { next.x += speed; p->direction =  1; moving = true; }
            if (IsKeyDown(KEY_LEFT))  { next.x -= speed; p->direction = -1; moving = true; }
            if (IsKeyDown(KEY_UP))    { next.y -= speed; moving = true; }
            if (IsKeyDown(KEY_DOWN))  { next.y += speed; moving = true; }
        }
        if (IsKeyPressed(KEY_K) && p->state != STATE_ATTACK && !p->isBlocking) {
            p->state    = STATE_ATTACK;
            p->frame    = 0;
            p->animTime = 0;
        }
    }

    // ANIMAÇÃO 
    if (p->state != STATE_ATTACK)
    {
        if (p->isBlocking)
            p->state = STATE_BLOCK;
        else if (moving)
            p->state = STATE_WALK;
        else
            p->state = STATE_SPRITE;
    }

    Animation *anim;
    if (p->state == STATE_ATTACK)
        anim = &p->attackSprite;
    else if (p->state == STATE_WALK)
        anim = &p->walkSprite;
    else if (p->state == STATE_BLOCK)
        anim = &p->blockSprite;
    else
        anim = &p->sprite;
    
    int frameW = anim->texture.width / anim->cols;
    int frameH = anim->texture.height / anim->rows;


    p->animTime += GetFrameTime();

    int totalFrames = anim->cols * anim->rows;

    if (p->animTime >= animSpeed)
    {
        p->animTime = 0;
        p->frame++;

        if (p->frame >= totalFrames)
        {
            if (p->state == STATE_ATTACK)
            {
                // FINALIZA ATAQUE
                p->state = STATE_SPRITE;
            }

            p->frame = 0;
        }
    }



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

    if (p->isHit)
    {
        Vector2 knockNext = p->position;
        knockNext.x += p->velocity.x * GetFrameTime();
        knockNext.y += p->velocity.y * GetFrameTime();

        Rectangle kRectX = { knockNext.x,    p->position.y, (float)frameW, (float)frameH };
        Rectangle kRectY = { p->position.x,  knockNext.y,   (float)frameW, (float)frameH };

        bool kColX = false, kColY = false;

        for (int i = 0; i < wallCount; i++) {
            if (CheckCollisionRecs(kRectX, walls[i])) kColX = true;
            if (CheckCollisionRecs(kRectY, walls[i])) kColY = true;
        }

        if (!kColX) p->position.x = knockNext.x;
        if (!kColY) p->position.y = knockNext.y;

        p->hitTimer += GetFrameTime();
        if (p->hitTimer > 0.2f)
        {
            p->isHit    = false;
            p->hitTimer = 0;
            p->velocity = (Vector2){0, 0};
        }
    }
}
Rectangle GetPlayerRect(Player *p)
{
    Animation *anim;

    if (p->state == STATE_ATTACK)
        anim = &p->attackSprite;
    else if (p->state == STATE_WALK)
        anim = &p->walkSprite;
    else if (p->state == STATE_BLOCK)
        anim = &p->blockSprite;
    else
        anim = &p->sprite;

    int frameW = anim->texture.width / anim->cols;
    int frameH = anim->texture.height / anim->rows;

    return (Rectangle){
        p->position.x,
        p->position.y,
        frameW,
        frameH
    };
}