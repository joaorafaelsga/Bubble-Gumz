#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

typedef struct {
    Vector2 position;
    int direction;

    int frame;
    float animTime;

    Texture2D sprite;

    int cols;
    int rows;
    int totalFrames;

    int controls;

    Vector2 velocity;
    bool isHit;
    float hitTimer;
} Player;

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed);
Rectangle GetPlayerRect(Player * p);

#endif