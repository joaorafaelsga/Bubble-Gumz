#include <raylib.h>
#include "player.h"
#include "render.h"
#include "combat.h"
#include "projectile.h"

void ApplyHit(Player *target, Vector2 knockback)
{
    target->isHit = true;
    target->velocity = knockback;
    target->hitTimer = 0;
}

int main()
{
    InitWindow(800, 600, "Beat'em Up");
    
    Texture2D bg = LoadTexture("assets/cenarios/cenario1.png");
    //GUMZ ART
    Texture2D sprite1 = LoadTexture("assets/players/gumz.png"); 
    Texture2D p1_walk = LoadTexture("assets/players/GumzWalk.webp");
    Texture2D p1_attack = LoadTexture("assets/players/GumzAtk.png");
    
    //BUBBLES ART
    Texture2D sprite2 = LoadTexture("assets/players/bubble.png");

    Texture2D p2_attack = LoadTexture("assets/players/BubblesAtk.png"); 

    SetTextureFilter(sprite1, TEXTURE_FILTER_POINT);
    SetTextureFilter(sprite2, TEXTURE_FILTER_POINT);
Player p1 = {
    .position = {400,300},
    .direction = 1,
    .sprite = sprite1,
    .walkSprite = p1_walk,
    .attackSprite = p1_attack,
    .cols = 1,
    .rows = 10,
    .totalFrames = 6,
    .controls = 0,
    .state = STATE_SPRITE
};

Player p2 = {
    .position = {200,300},
    .direction = 1,
    .sprite = sprite2,

    .attackSprite = p2_attack,
    .cols = 1,
    .rows = 4,
    .totalFrames = 4,
    .controls = 1,
    .state = STATE_SPRITE
};
    Hitbox punch = {0};
    Projectile bullet = {0};

    Rectangle walls[4] = {
        {0, 80, 800, 20},
        {0, 580, 800, 10},
        {0, 0, 5, 600},
        {795, 0, 5, 600}
    };

    float moveSpeed = 200.0f;
    float animSpeed = 0.1f;

    while (!WindowShouldClose())
    {
        float speed = moveSpeed * GetFrameTime();

        if(IsKeyPressed(KEY_F11)){
            ToggleFullscreen();
        }
                
            // SOCO (P2)
        if (IsKeyPressed(KEY_K) && p2.state != STATE_ATTACK) {

            p2.state = STATE_ATTACK;
            p2.stateTimer = 0;

            punch.active = true;
            punch.timer = 0;
            punch.duration = 0.15f;

            punch.damage = 10;
            punch.knockback = (Vector2){ p2.direction * 300, -50 };

            punch.rect = (Rectangle){
                p2.position.x + (p2.direction == 1 ? 40 : -40),
                p2.position.y,
                40,
                40
            };
        }

        // PROJETIL (P1)
        if (IsKeyPressed(KEY_R) && !bullet.active && p1.state != STATE_ATTACK) {

            p1.state = STATE_ATTACK;
            p1.stateTimer = 0;

            bullet.active = true;
            bullet.timer = 0;
            bullet.lifetime = 1.0f;

            bullet.rect = (Rectangle){
                p1.position.x,
                p1.position.y,
                20,
                20
            };

            bullet.velocity = (Vector2){ p1.direction * 400, 0 };

            bullet.damage = 5;
            bullet.knockback = (Vector2){ p1.direction * 200, -30 };
        }

        UpdatePlayer(&p1, speed, walls, 4, animSpeed);
        UpdatePlayer(&p2, speed, walls, 4, animSpeed);

        UpdateHitbox(&punch);
        UpdateProjectile(&bullet);
        if (punch.active && CheckCollisionRecs(punch.rect, GetPlayerRect(&p1))) {
        ApplyHit(&p1, punch.knockback);
        punch.active = false;
        }

        if (bullet.active && CheckCollisionRecs(bullet.rect, GetPlayerRect(&p2))) {
            ApplyHit(&p2, bullet.knockback);
            bullet.active = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(bg,
            (Rectangle){0,0,bg.width,bg.height},
            (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()},
            (Vector2){0,0},0,WHITE);

        DrawPlayersSorted(&p1, &p2);

        DrawHitbox(punch);
        DrawProjectile(bullet);

        EndDrawing();
    }

            CloseWindow();
}