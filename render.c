#include "render.h"

void DrawPlayer(Player p)
{
     if (p.state == STATE_DEAD)
    {
        Animation anim = p.sprite;
        int frameW = anim.texture.width  / anim.cols;
        int frameH = anim.texture.height / anim.rows;

        Rectangle source = { 0, 0, frameW, frameH };
        if (p.direction == -1) { source.x = frameW; source.width = -frameW; }

        Rectangle dest = { p.position.x, p.position.y, frameW, frameH };
        DrawTexturePro(anim.texture, source, dest,(Vector2){0,0}, 0, Fade(WHITE, 0.4f));
        return;
    }

    Animation anim;

    if (p.state == STATE_ATTACK)
        anim = p.attackSprite;
    else if (p.state == STATE_WALK)
        anim = p.walkSprite;
     else if (p.state == STATE_BLOCK)
        anim = p.blockSprite;
    else
        anim = p.sprite;

    int frameW = anim.texture.width / anim.cols;
    int frameH = anim.texture.height / anim.rows;

    int row = p.frame / anim.cols;
    int col = p.frame % anim.cols;

    Rectangle source = {
        col * frameW,
        row * frameH,
        frameW,
        frameH
    };

    if (p.direction == -1)
    {
        source.x += frameW;
        source.width = -frameW;
    }

    Rectangle dest = {
        p.position.x,
        p.position.y,
        frameW,
        frameH
    };

    DrawTexturePro(anim.texture, source, dest, (Vector2){0,0}, 0, WHITE);
}

void DrawPlayersSorted(Player *p1, Player *p2)
{
    if (p1->position.y < p2->position.y)
    {
        DrawPlayer(*p1);
        DrawPlayer(*p2);
    }
    else
    {
        DrawPlayer(*p2);
        DrawPlayer(*p1);
    }
}