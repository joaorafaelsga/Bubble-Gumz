#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <stdbool.h>

typedef struct {
    Texture2D texture;
    int cols;
    int rows;
    int totalFrames;
} Animation;

typedef struct Player {
    Vector2 position;
    int direction;

    int frame;
    float animTime;

    Animation sprite;
    Animation walkSprite;
    Animation attackSprite;
    Animation blockSprite;

    int cols;
    int rows;
    int totalFrames;

    int controls;

    Vector2 velocity;
    bool isHit;
    float hitTimer;

    int state;
    float stateTimer;

    int hp;
    int maxHp;

    bool isBlocking;
    
} Player;

typedef enum {
    STATE_SPRITE,
    STATE_WALK,
    STATE_ATTACK,
    STATE_HIT,
    STATE_BLOCK,
} PlayerState;

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed);
Rectangle GetPlayerRect(Player *p);

#endif