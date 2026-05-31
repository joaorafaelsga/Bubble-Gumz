#include "enemy.h"
#include <stdlib.h>
#include <math.h>

#define ENEMY_SPEED      80.0f
#define MELEE_RANGE      50.0f
#define RANGED_RANGE    200.0f
#define BULLET_SPEED    300.0f
#define BULLET_DAMAGE    20
#define MELEE_DAMAGE     25
#define SEPARATION_RADIUS 40.0f
#define SEPARATION_FORCE  120.0f

//INIMIGOS
Enemy *CreateEnemy(Vector2 pos, EnemyType type, Texture2D sprite, Texture2D attackSprite)
{
    Enemy *e = (Enemy *)malloc(sizeof(Enemy));

    e->position      = pos;
    e->direction     = -1;
    e->hp            = (type == ENEMY_MELEE) ? 60 : 40;
    e->maxHp         = e->hp;
    e->type          = type;
    e->state         = ENEMY_STATE;
    e->attackTimer   = 0;
    e->attackCooldown= (type == ENEMY_MELEE) ? 1.0f : 2.0f;
    e->frame         = 0;
    e->animTime      = 0;
    e->active        = true;
    e->next          = NULL;

    int atkRows = (type == ENEMY_MELEE) ? 7 : 3;
    e->sprite       = (Animation){ sprite,       1, 7 };
    e->attackSprite = (Animation){ attackSprite, 1, atkRows };

    return e;
}

// Retorna o player mais próximo
static Player *GetNearestPlayer(Enemy *e, Player *p1, Player *p2)
{
    float d1 = fabsf(p1->position.x - e->position.x);
    float d2 = fabsf(p2->position.x - e->position.x);
    return (d1 < d2) ? p1 : p2;
}

static void SpawnBullet(EnemyBullet **bullets, Vector2 pos, int direction)
{
    EnemyBullet *b = (EnemyBullet *)malloc(sizeof(EnemyBullet));
    b->rect     = (Rectangle){ pos.x, pos.y + 20, 16, 16 };
    b->velocity = (Vector2){ direction * BULLET_SPEED, 0 };
    b->active   = true;
    b->next     = *bullets;
    *bullets    = b;
}

static void ApplyEnemyHit(Player *target, Vector2 knockback, int damage)
{
    if (target->isBlocking) return; // bloqueio cancela dano do inimigo
    target->isHit    = true;
    target->velocity = knockback;
    target->hitTimer = 0;
    target->hp      -= damage;
    if (target->hp < 0) target->hp = 0;
}

void UpdateEnemies(Enemy *head, Player *p1, Player *p2,
                   EnemyBullet **bullets, Rectangle *walls, int wallCount)
{
    int idx = 0;

    for (Enemy *e = head; e != NULL; e = e->next, idx++)
    {
        if (!e->active || e->state == ENEMY_STATE_DEAD) continue;

        if (e->hp <= 0) {
            e->state  = ENEMY_STATE_DEAD;
            e->active = false;
            continue;
        }

        //Distribui alvos 
        Player *target = (idx % 2 == 0) ? p1 : p2;

  
        if (target->hp <= 0)
            target = (target == p1) ? p2 : p1;

        float dx   = target->position.x - e->position.x;
        float dy   = target->position.y - e->position.y;
        float dist = sqrtf(dx*dx + dy*dy);

        e->direction = (dx > 0) ? 1 : -1;

        float range = (e->type == ENEMY_MELEE) ? MELEE_RANGE : RANGED_RANGE;

        e->attackTimer += GetFrameTime();

        //separação entre inimigos
        float sepX = 0, sepY = 0;
        for (Enemy *other = head; other != NULL; other = other->next)
        {
            if (other == e || !other->active) continue;

            float ox = e->position.x - other->position.x;
            float oy = e->position.y - other->position.y;
            float od = sqrtf(ox*ox + oy*oy);

            if (od < SEPARATION_RADIUS && od > 0)
            {
                float strength = (SEPARATION_RADIUS - od) / SEPARATION_RADIUS;
                sepX += (ox / od) * strength;
                sepY += (oy / od) * strength;
            }
        }

        if (dist > range)
        {
            e->state = ENEMY_STATE_WALK;


            float nx = dx / dist;
            float ny = dy / dist;

            float moveX = nx * ENEMY_SPEED + sepX * SEPARATION_FORCE;
            float moveY = ny * ENEMY_SPEED + sepY * SEPARATION_FORCE;

            Vector2 next = e->position;
            next.x += moveX * GetFrameTime();
            next.y += moveY * GetFrameTime();

            Animation *anim = &e->sprite;
            int fw = anim->texture.width  / anim->cols;
            int fh = anim->texture.height / anim->rows;

            Rectangle rX = { next.x,        e->position.y, fw, fh };
            Rectangle rY = { e->position.x, next.y,        fw, fh };

            bool colX = false;
            bool colY = false;

            for (int i = 0; i < wallCount; i++) {
                if (CheckCollisionRecs(rX, walls[i])) colX = true;
                if (CheckCollisionRecs(rY, walls[i])) colY = true;
            }

            if (!colX) e->position.x = next.x;
            if (!colY) e->position.y = next.y;
        }
        else
        {
            Vector2 next = e->position;
            next.x += sepX * SEPARATION_FORCE * GetFrameTime();
            next.y += sepY * SEPARATION_FORCE * GetFrameTime();

            Animation *anim = &e->sprite;
            int fw = anim->texture.width  / anim->cols;
            int fh = anim->texture.height / anim->rows;

            Rectangle rX = { next.x,        e->position.y, fw, fh };
            Rectangle rY = { e->position.x, next.y,        fw, fh };

            bool colX = false;
            bool colY = false;

            for (int i = 0; i < wallCount; i++) {
                if (CheckCollisionRecs(rX, walls[i])) colX = true;
                if (CheckCollisionRecs(rY, walls[i])) colY = true;
            }

            if (!colX) e->position.x = next.x;
            if (!colY) e->position.y = next.y;

            if (e->attackTimer >= e->attackCooldown)
            {
                e->state       = ENEMY_STATE_ATTACK;
                e->attackTimer = 0;
                e->frame       = 0;
                e->animTime    = 0;

                if (e->type == ENEMY_MELEE)
                {
                    Rectangle attackRect = {
                        e->position.x + (e->direction == 1 ? 40 : -40),
                        e->position.y + 20, 40, 40
                    };
                    if (CheckCollisionRecs(attackRect, GetPlayerRect(target)))
                        ApplyEnemyHit(target,
                            (Vector2){ e->direction * 250, -40 }, MELEE_DAMAGE);
                }
                else
                {
                    SpawnBullet(bullets, e->position, e->direction);
                }
            }
            else
            {
                e->state = ENEMY_STATE;
            }
        }

        // Animação
        Animation *anim = (e->state == ENEMY_STATE_ATTACK)
                        ? &e->attackSprite
                        : &e->sprite;

        int totalFrames = anim->cols * anim->rows;

        e->animTime += GetFrameTime();
        if (e->animTime >= 0.12f) {
            e->animTime = 0;
            e->frame    = (e->frame + 1) % totalFrames;

            if (e->state == ENEMY_STATE_ATTACK && e->frame == 0)
                e->state = ENEMY_STATE;
        }
    }
}

void DrawEnemies(Enemy *head)
{
    for (Enemy *e = head; e != NULL; e = e->next)
    {
        if (!e->active) continue;

        Animation *anim = (e->state == ENEMY_STATE_ATTACK)
                        ? &e->attackSprite
                        : &e->sprite;

        int fw = anim->texture.width  / anim->cols;
        int fh = anim->texture.height / anim->rows;

        int row = e->frame / anim->cols;
        int col = e->frame % anim->cols;

        Rectangle source = { col * fw, row * fh, fw, fh };
        if (e->direction == 1) { source.x += fw; source.width = -fw; }

        Rectangle dest = { e->position.x, e->position.y, fw, fh };
        DrawTexturePro(anim->texture, source, dest, (Vector2){0,0}, 0, WHITE);

        // Barra de HP
        float pct = (float)e->hp / e->maxHp;
        DrawRectangle(e->position.x, e->position.y - 10, fw, 5, GRAY);
        DrawRectangle(e->position.x, e->position.y - 10, (int)(fw * pct), 5, RED);
    }
}

int CountActiveEnemies(Enemy *head)
{
    int count = 0;
    for (Enemy *e = head; e != NULL; e = e->next)
        if (e->active) count++;
    return count;
}

void FreeEnemies(Enemy **head)
{
    Enemy *cur = *head;
    while (cur) {
        Enemy *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    *head = NULL;
}

// PROJÉTEIS DOS INIMIGOS

void UpdateEnemyBullets(EnemyBullet **head, Player *p1, Player *p2)
{
    EnemyBullet *cur  = *head;
    EnemyBullet *prev = NULL;

    while (cur)
    {
        cur->rect.x += cur->velocity.x * GetFrameTime();

        // Acerta player
        bool hit = false;
        if (CheckCollisionRecs(cur->rect, GetPlayerRect(p1)))
            { ApplyEnemyHit(p1, cur->velocity, BULLET_DAMAGE); hit = true; }
        else if (CheckCollisionRecs(cur->rect, GetPlayerRect(p2)))
            { ApplyEnemyHit(p2, cur->velocity, BULLET_DAMAGE); hit = true; }

        // Sai da tela
        if (cur->rect.x < 0 || cur->rect.x > 800) hit = true;

        if (hit) {
            if (prev) prev->next = cur->next;
            else      *head      = cur->next;
            EnemyBullet *tmp = cur;
            cur = cur->next;
            free(tmp);
        } else {
            prev = cur;
            cur  = cur->next;
        }
    }
}

void DrawEnemyBullets(EnemyBullet *head)
{
    for (EnemyBullet *b = head; b != NULL; b = b->next)
        DrawRectangleRec(b->rect, ORANGE);
}

void FreeEnemyBullets(EnemyBullet **head)
{
    EnemyBullet *cur = *head;
    while (cur) {
        EnemyBullet *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    *head = NULL;
}