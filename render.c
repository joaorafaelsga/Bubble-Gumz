#include "render.h"

void DrawPlayer(Player p)
{
    Texture2D current;

    if (p.state == STATE_ATTACK)
        current = p.attackSprite;
    else if (p.state == STATE_WALK)
        current = p.walkSprite;
    else
        current = p.sprite;

    int frameW = current.width / p.cols;
    int frameH = current.height / p.rows;

    int row = p.frame / p.cols;
    int col = p.frame % p.cols;

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

    DrawTexturePro(current, source, dest, (Vector2){0,0}, 0, WHITE);
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