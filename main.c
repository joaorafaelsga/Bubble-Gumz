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
#include "save_system.h" 
#include "cutscene.h"  

#define MAX_SCORES 5
char topNames[MAX_SCORES][20];
int topScores[MAX_SCORES];

void LoadScores() {
    FILE *file = fopen("scores.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            if (fscanf(file, "%19s %d", topNames[i], &topScores[i]) != 2) {
                strcpy(topNames[i], "---"); topScores[i] = 0;
            }
        }
        fclose(file);
    } else {
        for (int i = 0; i < MAX_SCORES; i++) { strcpy(topNames[i], "---"); topScores[i] = 0; }
    }
}

void SaveScores() {
    FILE *file = fopen("scores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < MAX_SCORES; i++) fprintf(file, "%s %d\n", topNames[i], topScores[i]);
        fclose(file);
    }
}

void ApplyHit(Player *target, Vector2 knockback, int damage) {
    target->isHit = true; target->velocity = knockback;
    target->hitTimer = 0; target->hp -= damage;
}

int main()
{
    InitWindow(800, 600, "Beat'em Up - Bubble Gumz");

    Cutscene cutscene; InitCutscene(&cutscene); 
    LoadScores();
    DamageNode *damageList = NULL;
    GameSave *historyList = LoadFromFile(); 
    bool showSaveMessage = false; float messageTimer = 0.0f;
    
    Texture2D bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
    Texture2D menuBg = LoadTexture("assets/cenarios/Cenario2.png");
    Texture2D sprite1 = LoadTexture("assets/players/gumz.png"); 
    Texture2D p1_walk = LoadTexture("assets/players/GumzWalk.png");
    Texture2D p1_attack = LoadTexture("assets/players/GumzAtk.png");
    Texture2D p1_block = LoadTexture("assets/Enemies/Enemy1.png");
    Texture2D sprite2 = LoadTexture("assets/players/bubble.png");
    Texture2D p2_walk = LoadTexture("assets/players/BubblesWalk.png");
    Texture2D p2_attack = LoadTexture("assets/players/BubblesAtk.png"); 
    Texture2D p2_block = LoadTexture("assets/players/BubblesBlock.png");
    Texture2D particleSheet = LoadTexture("assets/particles/p-atck.png");
    
    Texture2D enemy1Sprite = LoadTexture("assets/Enemies/Enemy1.png");
    Texture2D enemy1Atk    = LoadTexture("assets/Enemies/Enemy1.png");
    Texture2D enemy2Sprite = LoadTexture("assets/Enemies/Enemy2.png");
    Texture2D enemy2Atk    = LoadTexture("assets/Enemies/Enemy2Atk.png");
    Texture2D bossSprite   = enemy1Sprite; 
    Texture2D bossAtk      = enemy1Atk;

    SetTextureFilter(sprite1, TEXTURE_FILTER_POINT);
    SetTextureFilter(sprite2, TEXTURE_FILTER_POINT);

    Menu menu; InitMenu(&menu, menuBg);
    GameScreen currentScreen = SCREEN_MENU;
    int currentFase = 1;
    int currentScore = 0;
    
    Player p1 = {0}; p1.position = (Vector2){ 400.0f, 300.0f }; p1.direction = 1; p1.controls = 0; p1.state = STATE_SPRITE; p1.hp = 100; p1.maxHp = 100; p1.sprite = (Animation){ sprite1, 1, 10, 10 }; p1.walkSprite = (Animation){ p1_walk, 1, 3, 3 }; p1.attackSprite = (Animation){ p1_attack, 1, 2, 2 }; p1.isDashing = false; p1.dashTimer = 0; p1.dashCooldown = 0; p1.dashSpeed = 500.0f;
    Player p2 = {0}; p2.position = (Vector2){ 200.0f, 300.0f }; p2.direction = 1; p2.controls = 1; p2.state = STATE_SPRITE; p2.hp = 100; p2.maxHp = 100; p2.sprite = (Animation){ sprite2, 1, 4, 4 }; p2.walkSprite = (Animation){ p2_walk, 1, 3, 3 }; p2.attackSprite = (Animation){ p2_attack, 1, 5, 5 }; p2.blockSprite = (Animation){ p2_block, 1, 1, 1 };

    Hitbox punch = {0}; Projectile bullet = {0}; bullet.texture = particleSheet; punch.texture  = particleSheet;
    Rectangle walls[4] = {{0, 80, 800, 20}, {0, 580, 800, 10}, {0, 0, 5, 600}, {795, 0, 5, 600}};
    float moveSpeed = 200.0f; float animSpeed = 0.1f;

    Enemy *enemies = NULL; EnemyBullet *enemyBullets = NULL;
    WaveSystem ws;
    
    InitWaves(&ws, 2);
    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);

    bool gameOver = false; 
    int winner = 0; 

    while (!WindowShouldClose())
    {
        float speed = moveSpeed * GetFrameTime();
        if(IsKeyPressed(KEY_F11)) ToggleFullscreen();
        
        // MENU 
        if (currentScreen == SCREEN_MENU)
        {
            int savedFase = LoadGame();
            if (historyList != NULL && savedFase < 1) savedFase = 1;

            if (IsKeyPressed(KEY_DOWN)) menu.currentItem = (menu.currentItem + 1) % MENU_COUNT;
            if (IsKeyPressed(KEY_UP))   menu.currentItem = (menu.currentItem - 1 + MENU_COUNT) % MENU_COUNT;

            if (IsKeyPressed(KEY_SPACE))
            {
                if (menu.currentItem == MENU_PLAY || menu.currentItem == MENU_SAVES) 
                {
                    FreeSaves(&historyList);            
                    historyList = LoadFromFile();
                    
                    if (historyList != NULL) {
                        LoadLatestSave(historyList, &p1, &p2);
                        p1.isDead = false; p2.isDead = false; gameOver = false; winner = 0;
                        
                        FILE *sf = fopen("score_save.txt", "r");
                        if(sf) { fscanf(sf, "%d", &currentScore); fclose(sf); }
                        else { currentScore = 0; }
                        
                        int sfFase = LoadGame(); if (sfFase > 0) currentFase = sfFase; else currentFase = 1;
                        if (currentFase == 2) bg = LoadTexture("assets/cenarios/Cenario2.png");
                        else bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");

                        FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets); enemies = NULL; enemyBullets = NULL;
                        InitWaves(&ws, 2);
                        SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);
                        currentScreen = SCREEN_GAME; 
                    } else {
                        if (menu.currentItem == MENU_PLAY) menu.currentItem = MENU_NEWGAME; 
                    }
                }
                
                if (menu.currentItem == MENU_NEWGAME) 
                {
                    DeleteSave(); FILE *fClear = fopen("savegame.txt", "w"); if (fClear) fclose(fClear); FreeSaves(&historyList);
                    remove("score_save.txt"); 
                    
                    currentFase = 1; currentScore = 0;
                    p1.hp = p1.maxHp; p1.position = (Vector2){400.0f, 300.0f}; p1.isHit = false; p1.state = STATE_SPRITE; p1.isDead = false;
                    p2.hp = p2.maxHp; p2.position = (Vector2){200.0f, 300.0f}; p2.isHit = false; p2.state = STATE_SPRITE; p2.isDead = false;
                    gameOver = false; winner = 0;
                    
                    InitCutscene(&cutscene);
                    bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                    FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets); enemies = NULL; enemyBullets = NULL;
                    InitWaves(&ws, 2);
                    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);
                    currentScreen = SCREEN_CUTSCENE; 
                }
                else if (menu.currentItem == MENU_QUIT) CloseWindow();
            }

            if (currentScreen == SCREEN_MENU) {
                BeginDrawing(); ClearBackground(BLACK); DrawMenu(menu, topScores[0], topNames[0], savedFase); EndDrawing();
                continue; 
            }
        }

        if (currentScreen == SCREEN_CUTSCENE) {
            UpdateCutscene(&cutscene);
            if (cutscene.finished) currentScreen = SCREEN_GAME;
            BeginDrawing(); ClearBackground(BLACK); DrawCutscene(cutscene); EndDrawing();
            continue;
        }
        
        if (IsKeyPressed(KEY_ONE)) { 
            AddSaveState(&historyList, p1, p2); SaveToFile(historyList); SaveGame(currentFase);         
            FILE *sf = fopen("score_save.txt", "w"); if(sf) { fprintf(sf, "%d", currentScore); fclose(sf); }
            SaveScores(); showSaveMessage = true; messageTimer = 2.0f; 
        }

        if (IsKeyPressed(KEY_TWO)) { 
            FreeSaves(&historyList); historyList = LoadFromFile();       
            if (LoadLatestSave(historyList, &p1, &p2)) {
                p1.isDead = false; p2.isDead = false; gameOver = false; winner = 0;
                FILE *sf = fopen("score_save.txt", "r"); if(sf) { fscanf(sf, "%d", &currentScore); fclose(sf); }
                int sfFase = LoadGame();
                if (sfFase > 0 && sfFase != currentFase) {
                    currentFase = sfFase;
                    if (currentFase == 2) bg = LoadTexture("assets/cenarios/Cenario2.png");
                    else bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                    FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets); enemies = NULL; enemyBullets = NULL;
                    InitWaves(&ws, 2);
                    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);
                }
            }
            showSaveMessage = true; messageTimer = 2.0f;
        }

        if (IsKeyPressed(KEY_M)) currentScreen = SCREEN_MENU;

        if (currentScore > topScores[0]) { topScores[0] = currentScore; strcpy(topNames[0], "P1"); }

        // UPDATES DA ENGINE 
        UpdateEnemies(enemies, &p1, &p2, &enemyBullets, walls, 4);
        UpdateEnemyBullets(&enemyBullets, &p1, &p2);
        UpdateWaves(&ws, &enemies, &enemyBullets, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase, &p1, &p2);

        if (IsFaseComplete(ws)) {   
            if (currentFase == 1) {
                currentFase = 2; bg = LoadTexture("assets/cenarios/Cenario2.png"); FreeEnemyBullets(&enemyBullets);
                InitWaves(&ws, 2); SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);
                SaveGame(currentFase);
                if (p1.isDead) { p1.isDead = false; p1.hp = p1.maxHp / 2; p1.state = STATE_SPRITE; }
                if (p2.isDead) { p2.isDead = false; p2.hp = p2.maxHp / 2; p2.state = STATE_SPRITE; }
            } else if (currentFase == 2 && !gameOver) {
                gameOver = true; winner = 3; SaveScores(); 
            }
        }
        
        if (showSaveMessage) { messageTimer -= GetFrameTime(); if (messageTimer <= 0) showSaveMessage = false; }
        
        // LÓGICA DE COMBATE 
        if (!gameOver) 
        {
            if (!p2.isDead && IsKeyPressed(KEY_K) && p2.state != STATE_ATTACK) {
                p2.state = STATE_ATTACK; p2.stateTimer = 0; p2.frame = 0; p2.animTime = 0;
                punch.active = true; punch.timer = 0; punch.duration = 0.15f; punch.damage = 15;
                punch.knockback = (Vector2){ p2.direction * 300, -50 };
                punch.rect = (Rectangle){ p2.position.x + (p2.direction == 1 ? 40 : -20), p2.position.y + 35, 40, 40 };
                punch.owner = &p2; 
            }

            if (!p1.isDead && IsKeyPressed(KEY_R) && !bullet.active && p1.state != STATE_ATTACK) {
                p1.state = STATE_ATTACK; p1.stateTimer = 0; p1.frame = 0; p1.animTime = 0;
                bullet.active = true; bullet.timer = 0; bullet.lifetime = 1.0f; bullet.damage = 10;
                bullet.velocity = (Vector2){ p1.direction * 400, 0 }; bullet.knockback = (Vector2){ p1.direction * 200, -30 };
                bullet.rect = (Rectangle){ p1.position.x + (p1.direction == 1 ? 44 : -20), p1.position.y + 25, 20, 20 };
            }
           
            // SOCOS BUBBLES
            if (punch.active) {
                for (Enemy *e = enemies; e != NULL; e = e->next) {
                    if (!e->active) continue;
                    
                    if (e->type == ENEMY_BOSS) {
                        // Bubbles soco no Boss (SÓ LEVA DANO SE O BRAÇO ESTIVER LEVANTADO E ACERTAR NO CORPO)
                        if (e->isVulnerable && CheckCollisionRecs(punch.rect, e->bodyRect)) {
                            e->hp -= punch.damage; e->isHit = true; e->hitTimer = 0;
                            if (e->hp <= 0) { e->hp = 0; e->state = ENEMY_STATE_DEAD; e->active = false; currentScore += 500; }
                            AddDamageText(&damageList, punch.damage, (Vector2){ e->bodyRect.x + 20, e->bodyRect.y + 100 });
                            punch.active = false; break;
                        } else if (CheckCollisionRecs(punch.rect, e->wheel1Rect) || CheckCollisionRecs(punch.rect, e->wheel2Rect) || CheckCollisionRecs(punch.rect, e->bodyRect)) {
                            AddDamageText(&damageList, 0, (Vector2){ punch.rect.x, punch.rect.y }); // Bloqueado!
                            punch.active = false; break;
                        }
                    } else {
                        // Inimigos Normais
                        Animation *anim = &e->sprite; int fw = anim->texture.width / anim->cols; int fh = anim->texture.height / anim->rows;
                        Rectangle er = { e->position.x, e->position.y, fw, fh };
                        if (CheckCollisionRecs(punch.rect, er)) {
                            e->hp -= punch.damage; e->velocity = (Vector2){ punch.owner->direction * 200, -30 }; e->isHit = true; e->hitTimer = 0;
                            if (e->hp <= 0) { e->hp = 0; e->state = ENEMY_STATE_DEAD; e->active = false; currentScore += 10; }
                            AddDamageText(&damageList, punch.damage, (Vector2){ e->position.x + 10, e->position.y - 20 });
                            punch.active = false; break;
                        }
                    }
                }
            }
            
            // BALAS GUMZ
            if (bullet.active) {
                for (Enemy *e = enemies; e != NULL; e = e->next) {
                    if (!e->active) continue;

                    if (e->type == ENEMY_BOSS) {
                        // Balas grudam nas rodas!
                        bool hitSomething = false;
                        if (e->wheel1Hp > 0 && CheckCollisionRecs(bullet.rect, e->wheel1Rect)) {
                            e->wheel1Hp--; hitSomething = true;
                            AddDamageText(&damageList, 1, (Vector2){ e->wheel1Rect.x + 40, e->wheel1Rect.y });
                        } else if (e->wheel2Hp > 0 && CheckCollisionRecs(bullet.rect, e->wheel2Rect)) {
                            e->wheel2Hp--; hitSomething = true;
                            AddDamageText(&damageList, 1, (Vector2){ e->wheel2Rect.x + 40, e->wheel2Rect.y });
                        } else if (CheckCollisionRecs(bullet.rect, e->bodyRect) || CheckCollisionRecs(bullet.rect, e->wheel1Rect) || CheckCollisionRecs(bullet.rect, e->wheel2Rect)) {
                            AddDamageText(&damageList, 0, (Vector2){ bullet.rect.x, bullet.rect.y }); // Dano bloqueado
                            hitSomething = true;
                        }

                        if (hitSomething) { bullet.active = false; break; }
                    } else {
                        // Inimigos Normais
                        Animation *anim = &e->sprite; int fw = anim->texture.width / anim->cols; int fh = anim->texture.height / anim->rows;
                        Rectangle er = { e->position.x, e->position.y, fw, fh };
                        if (CheckCollisionRecs(bullet.rect, er)) {
                            e->hp -= bullet.damage; e->velocity = (Vector2){ p1.direction * 150, -20 }; e->isHit = true; e->hitTimer = 0;
                            if (e->hp <= 0) { e->hp = 0; e->state = ENEMY_STATE_DEAD; e->active = false; currentScore += 10; }
                            AddDamageText(&damageList, bullet.damage, (Vector2){ e->position.x + 10, e->position.y - 20 });
                            bullet.active = false; break;
                        }
                    }
                }
            }
            UpdatePlayer(&p1, speed, walls, 4, animSpeed); UpdatePlayer(&p2, speed, walls, 4, animSpeed);
            UpdateHitbox(&punch); UpdateProjectile(&bullet);

            if (p1.hp <= 0 && !p1.isDead) { p1.isDead = true; p1.hp = 0; p1.state = STATE_DEAD; p1.velocity = (Vector2){0, 0}; p1.isHit = false; }
            if (p2.hp <= 0 && !p2.isDead) { p2.isDead = true; p2.hp = 0; p2.state = STATE_DEAD; p2.velocity = (Vector2){0, 0}; p2.isHit = false; }
            if (p1.isDead && p2.isDead) { gameOver = true; winner = 0; SaveScores(); }
        }
        else 
        {
            if (IsKeyPressed(KEY_SPACE)) {
                p1.hp = p1.maxHp; p1.position = (Vector2){400.0f, 300.0f}; p1.state = STATE_SPRITE; p1.isHit = false; p1.isDead = false;
                p2.hp = p2.maxHp; p2.position = (Vector2){200.0f, 300.0f}; p2.state = STATE_SPRITE; p2.isHit = false; p2.isDead = false;
                punch.active = false; bullet.active = false; FreeDamageTexts(&damageList); gameOver = false; winner = 0;

                currentFase = 1; currentScore = 0; remove("score_save.txt"); 
                DeleteSave(); FILE *fClear = fopen("savegame.txt", "w"); if (fClear) fclose(fClear); FreeSaves(&historyList);

                bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets); enemies = NULL; enemyBullets = NULL;
                InitWaves(&ws, 2); SpawnWave(&ws, &enemies,enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, bossSprite, bossAtk, currentFase);

                currentScreen = SCREEN_MENU;
            }
        }
        
        // RENDERIZAÇÃO
        BeginDrawing(); ClearBackground(RAYWHITE);
        DrawTexturePro(bg, (Rectangle){0,0,bg.width,bg.height}, (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()}, (Vector2){0,0},0,WHITE);

        DrawPlayersSorted(&p1, &p2); DrawEnemies(enemies); DrawEnemyBullets(enemyBullets); DrawWaveInfo(ws);
        DrawHitbox(punch, p2.direction); DrawProjectile(bullet, p1.direction);

        DrawHealthBar(20, 30, p1.hp, p1.maxHp, GREEN, "GUMZ (P1)");
        DrawHealthBar(480, 30, p2.hp, p2.maxHp, BLUE, "BUBBLES (P2)");
        
        DrawText(TextFormat("SCORE: %04d", currentScore), 340, 20, 24, BLACK);
        
        // BARRA DO BOSS
        for (Enemy *e = enemies; e != NULL; e = e->next) {
            if (e->active && e->type == ENEMY_BOSS) {
                DrawText("TRATOR GIGANTE", 310, 60, 20, RED);
                DrawRectangle(200, 85, 400, 20, DARKGRAY);
                float pct = (float)e->hp / e->maxHp;
                DrawRectangle(200, 85, (int)(400 * pct), 20, RED);
                break;
            }
        }

        UpdateAndDrawDamageTexts(&damageList);

        if (showSaveMessage && historyList != NULL) {
            const char* currentSaveName = saveNames[(historyList->id - 1) % 10]; 
            DrawText(TextFormat("%s Salvo/Carregado!", currentSaveName), 300, 125, 20, DARKBLUE);
        }
        DrawText(TextFormat("Top Score: %s - %d pts", topNames[0], topScores[0]), 300, 560, 20, WHITE);

        if (gameOver) {
            DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.7f));
            if (winner == 3) {
                DrawText("VITÓRIA! O TRATOR FOI DESTRUÍDO!", 70, 220, 36, GOLD);
                DrawText(TextFormat("SCORE FINAL: %d", currentScore), 280, 280, 24, WHITE);
            } else {
                DrawText("GAME OVER", 280, 250, 40, RED);
            }
            DrawText("Pressione SPACE para voltar ao Menu", 200, 350, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    SaveScores(); FreeDamageTexts(&damageList); FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets); FreeSaves(&historyList); FreeCutscene(&cutscene);
    CloseWindow(); return 0;
}