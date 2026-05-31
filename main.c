#include <raylib.h>
#include <stdio.h>
#include <string.h>

#include "player.h"
#include "render.h"
#include "combat.h"
#include "projectile.h"
#include "ui.h"
#include "menu.h"
#include "wave.h"
#include "enemy.h"

// (Sistema de Scores)
#define MAX_SCORES 5
char topNames[MAX_SCORES][20];
int topScores[MAX_SCORES];

void LoadScores() {
    FILE *file = fopen("scores.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            if (fscanf(file, "%19s %d", topNames[i], &topScores[i]) != 2) {
                strcpy(topNames[i], "---");
                topScores[i] = 0;
            }
        }
        fclose(file);
    } else {
        for (int i = 0; i < MAX_SCORES; i++) {
            strcpy(topNames[i], "---");
            topScores[i] = 0;
        }
    }
}

void SaveScores() {
    FILE *file = fopen("scores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            fprintf(file, "%s %d\n", topNames[i], topScores[i]);
        }
        fclose(file);
    }
}

// Alterado para receber e aplicar o dano à variável HP
void ApplyHit(Player *target, Vector2 knockback, int damage)
{
    target->isHit = true;
    target->velocity = knockback;
    target->hitTimer = 0;

    target->hp -= damage;
}

int main()
{
    InitWindow(800, 600, "Beat'em Up");

    LoadScores();
    DamageNode *damageList = NULL;
    
    //CENARIOS
    Texture2D bg = LoadTexture("assets/cenarios/cenario1_2.0.png");
    Texture2D menuBg = LoadTexture("assets/cenarios/Cenario2.png");
    //GUMZ ART
    Texture2D sprite1 = LoadTexture("assets/players/gumz.png"); 
    Texture2D p1_walk = LoadTexture("assets/players/GumzWalk.png");
    Texture2D p1_attack = LoadTexture("assets/players/GumzAtk.png");
    Texture2D p1_block = LoadTexture("assets/Enemies/Enemy1.png");

    //BUBBLES ART
    Texture2D sprite2 = LoadTexture("assets/players/bubble.png");
    Texture2D p2_walk = LoadTexture("assets/players/BubblesWalk.png");
    Texture2D p2_attack = LoadTexture("assets/players/BubblesAtk.png"); 
    Texture2D p2_block = LoadTexture("assets/players/BubblesBlock.png");


    Texture2D particleSheet = LoadTexture("assets/particles/p-atck.png");
    //INIMIGOS
    Texture2D enemy1Sprite = LoadTexture("assets/Enemies/Enemy1.png");
    Texture2D enemy1Atk    = LoadTexture("assets/Enemies/Enemy1.png");
    Texture2D enemy2Sprite = LoadTexture("assets/Enemies/Enemy2.png");
    Texture2D enemy2Atk    = LoadTexture("assets/Enemies/Enemy2Atk.png");

    SetTextureFilter(sprite1, TEXTURE_FILTER_POINT);
    SetTextureFilter(sprite2, TEXTURE_FILTER_POINT);

    Menu menu;
    InitMenu(&menu, menuBg);
    GameScreen currentScreen = SCREEN_MENU;
    int currentFase = 1;
    
    //Gumz
    Player p1 = {0};
    p1.position = (Vector2){400,300};
    p1.direction = 1;
    p1.controls = 0;
    p1.state = STATE_SPRITE;
    p1.hp = 100;
    p1.maxHp = 100;
    

    p1.sprite = (Animation){ sprite1, 1, 10 };     
    p1.walkSprite = (Animation){ p1_walk, 1, 3 };  
    p1.attackSprite = (Animation){ p1_attack, 1, 2 };
    p1.blockSprite = (Animation){ p1_block, 1, 7 };

    //Bubbles
    Player p2 = {0};
    p2.position = (Vector2){200,300};
    p2.direction = 1;
    p2.controls = 1;
    p2.state = STATE_SPRITE;
    p2.hp = 100;
    p2.maxHp = 100;

    p2.sprite = (Animation){ sprite2, 1, 4 };
    p2.walkSprite = (Animation){ p2_walk, 1, 3 }; 
    p2.attackSprite = (Animation){ p2_attack, 1, 5 };
    p2.blockSprite = (Animation){ p2_block, 1, 1 };

    Hitbox punch = {0};
    Projectile bullet = {0};
    bullet.texture = particleSheet;
    punch.texture  = particleSheet;

    Rectangle walls[4] = {
        {0, 80, 800, 20},
        {0, 580, 800, 10},
        {0, 0, 5, 600},
        {795, 0, 5, 600}
    };

    float moveSpeed = 200.0f;
    float animSpeed = 0.1f;

    Enemy       *enemies     = NULL;
    EnemyBullet *enemyBullets = NULL;

    WaveSystem ws;
    int totalWaves = (currentFase == 1) ? 2 : 1;
    InitWaves(&ws, totalWaves);
    SpawnWave(&ws, &enemies,
            enemy1Sprite, enemy1Atk,
            enemy2Sprite, enemy2Atk, currentFase);

    while (!WindowShouldClose())
    {
        float speed = moveSpeed * GetFrameTime();

        if(IsKeyPressed(KEY_F11)){
            ToggleFullscreen();
        }
        //MENU
        if (currentScreen == SCREEN_MENU)
        {
            if (menu.currentItem == MENU_NEWGAME && IsKeyPressed(KEY_ENTER))
            {
                for (int i = 0; i < MAX_SCORES; i++) {
                    strcpy(topNames[i], "---");
                    topScores[i] = 0;
                }
                SaveScores();
                DeleteSave();
                currentFase = 0;
            }

            int savedFase = LoadGame();
            currentScreen = UpdateMenu(&menu);

            BeginDrawing();
            ClearBackground(BLACK);
            DrawMenu(menu, topScores[0], topNames[0], savedFase);
            EndDrawing();

            continue; 
        }

        
        // SOCO (P2)
        if (IsKeyPressed(KEY_K) && p2.state != STATE_ATTACK) {

            p2.state = STATE_ATTACK;
            p2.stateTimer = 0;
            p2.frame = 0;
            p2.animTime = 0;

            punch.active = true;
            punch.timer = 0;
            punch.duration = 0.15f;

            punch.damage = 10;
            punch.knockback = (Vector2){ p2.direction * 300, -50 };

            punch.rect = (Rectangle){
                p2.position.x + (p2.direction == 1 ? 40 : -20),
                p2.position.y + 35,
                40,
                40
            };

            punch.owner = &p2; 
        }

        // PROJETIL (P1)
        if (IsKeyPressed(KEY_R) && !bullet.active && p1.state != STATE_ATTACK) {

            p1.state = STATE_ATTACK;
            p1.stateTimer = 0;
            p1.frame = 0;
            p1.animTime = 0;

            bullet.active = true;
            bullet.timer = 0;
            bullet.lifetime = 1.0f;

            bullet.rect = (Rectangle){
                p1.position.x + (p1.direction == 1 ? 44 : -20),
                p1.position.y + 25,
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

        UpdateEnemies(enemies, &p1, &p2, &enemyBullets, walls, 4);
        UpdateEnemyBullets(&enemyBullets, &p1, &p2);
        UpdateWaves(&ws, &enemies, &enemyBullets,
                    enemy1Sprite, enemy1Atk,
                    enemy2Sprite, enemy2Atk, currentFase);

        if (IsFaseComplete(ws) && currentFase == 1)
        {
            currentFase = 2;
            bg = LoadTexture("assets/cenarios/cenario2.png");
            FreeEnemyBullets(&enemyBullets);
            InitWaves(&ws, 1);
            SpawnWave(&ws, &enemies,
                    enemy1Sprite, enemy1Atk,
                    enemy2Sprite, enemy2Atk, currentFase);
            SaveGame(currentFase);
        }
        

        if (punch.active && punch.owner != &p1 &&
            CheckCollisionRecs(punch.rect, GetPlayerRect(&p1)))
        {
            float attackDir = punch.owner->direction;
            bool fromFront  = (p1.direction == -attackDir);

            if (p1.isBlocking && fromFront) {
                // Bloqueio bem-sucedido — knockback leve
                p1.velocity.x = punch.knockback.x * 0.2f;
                p1.velocity.y = punch.knockback.y * 0.2f;
            } else {
                ApplyHit(&p1, punch.knockback, punch.damage);
                Vector2 textPos = { p1.position.x + 20, p1.position.y - 20 };
                AddDamageText(&damageList, punch.damage, textPos);
            }
            punch.active = false;
        }

        if (punch.active && punch.owner != &p2 &&
            CheckCollisionRecs(punch.rect, GetPlayerRect(&p2)))
        {
            float attackDir = punch.owner->direction;
            bool fromFront  = (p2.direction == -attackDir);

            if (p1.isBlocking && fromFront) {
                p1.velocity.x = punch.knockback.x * 0.2f;
                p1.velocity.y = punch.knockback.y * 0.2f;
                p1.isHit      = true;
                p1.hitTimer   = 0;
            } else {
                ApplyHit(&p2, punch.knockback, punch.damage);
                Vector2 textPos = { p2.position.x + 20, p2.position.y - 20 };
                AddDamageText(&damageList, punch.damage, textPos);
            }
            punch.active = false;
        }
        

        // Projétil — só acerta inimigos (quando implementar)
        if (bullet.active && CheckCollisionRecs(bullet.rect, GetPlayerRect(&p2)))
{
            bool fromFront = (p2.direction == -p1.direction);

            if (p2.isBlocking && fromFront) {
                p2.velocity.x = bullet.knockback.x * 0.2f;
                p2.velocity.y = bullet.knockback.y * 0.2f;
                p2.isHit      = true;
                p2.hitTimer   = 0;
            } else {
                ApplyHit(&p2, bullet.knockback, bullet.damage);
                Vector2 textPos = { p2.position.x + 20, p2.position.y - 20 };
                AddDamageText(&damageList, bullet.damage, textPos);
            }
            bullet.active = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(bg,
            (Rectangle){0,0,bg.width,bg.height},
            (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()},
            (Vector2){0,0},0,WHITE);

        DrawPlayersSorted(&p1, &p2);
        DrawEnemies(enemies);
        DrawEnemyBullets(enemyBullets);
        DrawWaveInfo(ws);

        DrawHitbox(punch, p2.direction);
        DrawProjectile(bullet, p1.direction);

        // INTERFACE (BARRAS DE VIDA E DANOS)
        DrawHealthBar(20, 30, p1.hp, p1.maxHp, GREEN, "GUMZ (P1)");
        DrawHealthBar(480, 30, p2.hp, p2.maxHp, BLUE, "BUBBLES (P2)");
        
        UpdateAndDrawDamageTexts(&damageList);

        // Score na tela
        DrawText(TextFormat("Top Score: %s - %d pts", topNames[0], topScores[0]), 300, 560, 20, WHITE);

        EndDrawing();
    }

    // Salvar no arquivo as pontuações e libertar os ponteiros alocados (free) da lista
    SaveScores();
    FreeDamageTexts(&damageList);
    
    FreeEnemies(&enemies);
    FreeEnemyBullets(&enemyBullets);
    
    CloseWindow();
    return 0;
}