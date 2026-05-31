#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <stdbool.h>
#include "player.h"

typedef enum {
    ENEMY_MELEE,
    ENEMY_RANGED
} EnemyType;

typedef enum {
    ENEMY_STATE,
    ENEMY_STATE_WALK,
    ENEMY_STATE_ATTACK,
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct Bullet {
    Rectangle rect;
    Vector2 velocity;
    bool active;
    struct Bullet *next;  //lista encadeada de projéteis
} EnemyBullet;

typedef struct Enemy {
    Vector2 position;
    int direction;
    int hp;
    int maxHp;

    EnemyType type;
    EnemyState state;

    float attackTimer;
    float attackCooldown;
    Vector2 velocity;
    bool    isHit; 
    float   hitTimer;

    // Animação
    Animation sprite;
    Animation attackSprite;
    int frame;
    float animTime;

    bool active;

    struct Enemy *next; 
} Enemy;

// Lista de inimigos
Enemy *CreateEnemy(Vector2 pos, EnemyType type, Texture2D sprite, Texture2D attackSprite);
void UpdateEnemies(Enemy *head, Player *p1, Player *p2,
                   EnemyBullet **bullets, Rectangle *walls, int wallCount);
void DrawEnemies(Enemy *head);
void FreeEnemies(Enemy **head);
int  CountActiveEnemies(Enemy *head);

// Lista de projéteis inimigos
void UpdateEnemyBullets(EnemyBullet **head, Player *p1, Player *p2);
void DrawEnemyBullets(EnemyBullet *head);
void FreeEnemyBullets(EnemyBullet **head);

#endif