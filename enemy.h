#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <stdbool.h>
#include "player.h"

typedef enum {
    ENEMY_MELEE,
    ENEMY_RANGED,
    ENEMY_BOSS // O nosso Trator Boss
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
    struct Bullet *next;  
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

    //VARIÁVEIS EXCLUSIVAS DO BOSS (TRATOR)
    int wheel1Hp;       // HP da Roda Superior (Tiros de Chiclete)
    int wheel2Hp;       // HP da Roda Inferior
    bool isVulnerable;  // Fica true quando as rodas param
    Rectangle wheel1Rect;
    Rectangle wheel2Rect;
    Rectangle bodyRect;
   

    Animation sprite;
    Animation attackSprite;
    int frame;
    float animTime;

    bool active;
    struct Enemy *next; 
} Enemy;

Enemy *CreateEnemy(Vector2 pos, EnemyType type, Texture2D sprite, Texture2D attackSprite);
void UpdateEnemies(Enemy *head, Player *p1, Player *p2, EnemyBullet **bullets, Rectangle *walls, int wallCount);
void DrawEnemies(Enemy *head);
void FreeEnemies(Enemy **head);
int  CountActiveEnemies(Enemy *head);

void UpdateEnemyBullets(EnemyBullet **head, Player *p1, Player *p2);
void DrawEnemyBullets(EnemyBullet *head);
void FreeEnemyBullets(EnemyBullet **head);

#endif