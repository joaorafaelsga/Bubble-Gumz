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

// MATRIZES DE SCORES
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

void ApplyHit(Player *target, Vector2 knockback, int damage)
{
    target->isHit = true;
    target->velocity = knockback;
    target->hitTimer = 0;
    target->hp -= damage;
}

// FUNÇÃO PRINCIPAL
int main()
{
    InitWindow(800, 600, "Beat'em Up - Bubble Gumz");

    Cutscene cutscene;
    InitCutscene(&cutscene); 

    LoadScores();
    DamageNode *damageList = NULL;
    
    // Inicia a lista encadeada e lê o histórico de saves do arquivo
    GameSave *historyList = LoadFromFile(); 
    bool showSaveMessage = false;
    float messageTimer = 0.0f;
    
    // TEXTURAS E CENÁRIOS
    Texture2D bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
    Texture2D menuBg = LoadTexture("assets/cenarios/Cenario2.png");
    
    // GUMZ ART
    Texture2D sprite1 = LoadTexture("assets/players/gumz.png"); 
    Texture2D p1_walk = LoadTexture("assets/players/GumzWalk.png");
    Texture2D p1_attack = LoadTexture("assets/players/GumzAtk.png");
    Texture2D p1_block = LoadTexture("assets/Enemies/Enemy1.png");

    // BUBBLES ART
    Texture2D sprite2 = LoadTexture("assets/players/bubble.png");
    Texture2D p2_walk = LoadTexture("assets/players/BubblesWalk.png");
    Texture2D p2_attack = LoadTexture("assets/players/BubblesAtk.png"); 
    Texture2D p2_block = LoadTexture("assets/players/BubblesBlock.png");

    Texture2D particleSheet = LoadTexture("assets/particles/p-atck.png");
    
    // INIMIGOS
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
    
    // Configuração Gumz
    Player p1 = {0}; 
    p1.position = (Vector2){ 400.0f, 300.0f }; 
    p1.direction = 1; 
    p1.controls = 0; 
    p1.state = STATE_SPRITE; 
    p1.hp = 100; 
    p1.maxHp = 100;
    p1.sprite = (Animation){ sprite1, 1, 10, 10 };     
    p1.walkSprite = (Animation){ p1_walk, 1, 3, 3 };  
    p1.attackSprite = (Animation){ p1_attack, 1, 2, 2 };
    p1.isDashing    = false;
    p1.dashTimer    = 0;
    p1.dashCooldown = 0;
    p1.dashSpeed    = 500.0f;

    // Configuração Bubbles
    Player p2 = {0};
    p2.position = (Vector2){ 200.0f, 300.0f }; 
    p2.direction = 1; 
    p2.controls = 1; 
    p2.state = STATE_SPRITE; 
    p2.hp = 100; 
    p2.maxHp = 100;
    p2.sprite = (Animation){ sprite2, 1, 4, 4 };
    p2.walkSprite = (Animation){ p2_walk, 1, 3, 3 }; 
    p2.attackSprite = (Animation){ p2_attack, 1, 5, 5 };
    p2.blockSprite = (Animation){ p2_block, 1, 1, 1 };

    Hitbox punch = {0};
    Projectile bullet = {0};
    bullet.texture = particleSheet;
    punch.texture  = particleSheet;

    Rectangle walls[4] = {{0, 80, 800, 20}, {0, 580, 800, 10}, {0, 0, 5, 600}, {795, 0, 5, 600}};

    float moveSpeed = 200.0f;
    float animSpeed = 0.1f;

    Enemy *enemies = NULL;
    EnemyBullet *enemyBullets = NULL;

    WaveSystem ws;
    int totalWaves = (currentFase == 1) ? 2 : 1;
    InitWaves(&ws, totalWaves);
    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);

    bool gameOver = false;
    int winner = 0; 

    while (!WindowShouldClose())
    {
        float speed = moveSpeed * GetFrameTime();

        if(IsKeyPressed(KEY_F11)) ToggleFullscreen();
        
        // ===============================================
        // TELA DE MENU INTELIGENTE (Corrigido)
        // ===============================================
        if (currentScreen == SCREEN_MENU)
        {
            int savedFase = LoadGame();
            if (historyList != NULL && savedFase < 1) savedFase = 1;

            // NAVEGAÇÃO DO MENU (Controlada diretamente aqui para evitar bugs)
            if (IsKeyPressed(KEY_DOWN)) menu.currentItem = (menu.currentItem + 1) % MENU_COUNT;
            if (IsKeyPressed(KEY_UP))   menu.currentItem = (menu.currentItem - 1 + MENU_COUNT) % MENU_COUNT;

            // QUANDO CARREGAR NO ESPAÇO
            if (IsKeyPressed(KEY_SPACE))
            {
                // Se escolher PLAY ou SAVES, o jogo vai carregar o seu save!
                if (menu.currentItem == MENU_PLAY || menu.currentItem == MENU_SAVES) 
                {
                    FreeSaves(&historyList);            
                    historyList = LoadFromFile();
                    
                    if (historyList != NULL) {
                        LoadLatestSave(historyList, &p1, &p2);
                        
                        // Garante que eles voltam vivos e em condições
                        p1.isDead = false;
                        p2.isDead = false;
                        gameOver = false; 
                        
                        // Sincroniza a fase e o cenário
                        int sf = LoadGame();
                        if (sf > 0) currentFase = sf;
                        else currentFase = 1;

                        if (currentFase == 2) bg = LoadTexture("assets/cenarios/Cenario2.png");
                        else bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");

                        // Recria as Waves com base na fase onde você salvou
                        FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets);
                        enemies = NULL; enemyBullets = NULL;
                        InitWaves(&ws, (currentFase == 1) ? 2 : 1);
                        SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);

                        currentScreen = SCREEN_GAME; // Entra direto na luta
                    } else {
                        // Se clicou Play mas NÃO existe save, transforma num Novo Jogo
                        if (menu.currentItem == MENU_PLAY) menu.currentItem = MENU_NEWGAME; 
                    }
                }
                
                // Se escolher NEW GAME (ou se clicou em Play sem ter nenhum Save)
                if (menu.currentItem == MENU_NEWGAME) 
                {
                    // Limpa absolutamente tudo para começar do zero
                    for (int i = 0; i < MAX_SCORES; i++) {
                        strcpy(topNames[i], "---");
                        topScores[i] = 0;
                    }
                    SaveScores();
                    DeleteSave(); // Apaga o save.dat

                    FILE *fClear = fopen("savegame.txt", "w");
                    if (fClear) fclose(fClear);
                    FreeSaves(&historyList);
                    
                    currentFase = 1;

                    p1.hp = p1.maxHp; p1.position = (Vector2){400.0f, 300.0f}; p1.isHit = false; p1.state = STATE_SPRITE; p1.isDead = false;
                    p2.hp = p2.maxHp; p2.position = (Vector2){200.0f, 300.0f}; p2.isHit = false; p2.state = STATE_SPRITE; p2.isDead = false;
                    gameOver = false;
                    
                    InitCutscene(&cutscene);

                    bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                    FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets);
                    enemies = NULL; enemyBullets = NULL;
                    InitWaves(&ws, 2);
                    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);

                    currentScreen = SCREEN_CUTSCENE; // Manda para a Cutscene
                }
                else if (menu.currentItem == MENU_QUIT) 
                {
                    CloseWindow();
                }
            }

            // O menu só é desenhado se não sairmos dele neste frame
            if (currentScreen == SCREEN_MENU) {
                BeginDrawing();
                ClearBackground(BLACK);
                // Usamos o DrawMenu original para renderizar as opções gráficas
                DrawMenu(menu, topScores[0], topNames[0], savedFase);
                EndDrawing();
                continue; 
            }
        }

        // ===============================================
        // CUTSCENE
        // ===============================================
        if (currentScreen == SCREEN_CUTSCENE)
        {
            UpdateCutscene(&cutscene);

            if (cutscene.finished)
                currentScreen = SCREEN_GAME;

            BeginDrawing();
            ClearBackground(BLACK);
            DrawCutscene(cutscene);
            EndDrawing();
            continue;
        }

        
        // ===============================================
        // SISTEMA DE SAVE/LOAD EM TEMPO REAL
        // ===============================================
        if (IsKeyPressed(KEY_ONE)) { // Tecla 1 para salvar rápido na luta
            AddSaveState(&historyList, p1, p2); 
            SaveToFile(historyList);  
            SaveGame(currentFase); // Salva também o número da fase          
            showSaveMessage = true;
            messageTimer = 2.0f; 
        }

        if (IsKeyPressed(KEY_TWO)) { // Tecla 2 para carregar manualmente
            FreeSaves(&historyList);            
            historyList = LoadFromFile();       
            if (LoadLatestSave(historyList, &p1, &p2)) {
                p1.isDead = false;
                p2.isDead = false;
                gameOver = false;
                // Sincroniza e restaura o fundo/fase/inimigos ao carregar a meio do jogo
                int sf = LoadGame();
                if (sf > 0 && sf != currentFase) {
                    currentFase = sf;
                    if (currentFase == 2) bg = LoadTexture("assets/cenarios/Cenario2.png");
                    else bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                    
                    FreeEnemies(&enemies); FreeEnemyBullets(&enemyBullets);
                    enemies = NULL; enemyBullets = NULL;
                    InitWaves(&ws, (currentFase == 1) ? 2 : 1);
                    SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);
                }
            }
            showSaveMessage = true;
            messageTimer = 2.0f;
        }

        if (IsKeyPressed(KEY_M)) { // Sair para o menu
            currentScreen = SCREEN_MENU;
        }

        // ===============================================
        // ATUALIZAÇÕES DA ENGINE (Inimigos, Fisicas, etc)
        // ===============================================

        UpdateEnemies(enemies, &p1, &p2, &enemyBullets, walls, 4);
        UpdateEnemyBullets(&enemyBullets, &p1, &p2);
        UpdateWaves(&ws, &enemies, &enemyBullets, enemy1Sprite, enemy1Atk,
        enemy2Sprite, enemy2Atk, currentFase, &p1, &p2);

        if (IsFaseComplete(ws) && currentFase == 1)
        {   
            currentFase = 2;
            bg = LoadTexture("assets/cenarios/Cenario2.png");
            FreeEnemyBullets(&enemyBullets);
            InitWaves(&ws, 1);
            SpawnWave(&ws, &enemies, enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);
            SaveGame(currentFase);

            if (p1.isDead) { 
                p1.isDead = false;
                p1.hp = p1.maxHp / 2; 
                p1.state = STATE_SPRITE; 
            }
            if (p2.isDead) { 
                p2.isDead = false; 
                p2.hp = p2.maxHp / 2; 
                p2.state = STATE_SPRITE; 
            }
        }
        
        if (punch.active && punch.owner != &p1 && CheckCollisionRecs(punch.rect, GetPlayerRect(&p1)))  
        {
            float attackDir = punch.owner->direction;
            bool fromFront  = (p1.direction == -attackDir);

            if (p1.isBlocking && fromFront) {
                p1.velocity.x = punch.knockback.x * 0.2f;
                p1.velocity.y = punch.knockback.y * 0.2f;
            } else {
                ApplyHit(&p1, punch.knockback, punch.damage);
                Vector2 textPos = { p1.position.x + 20, p1.position.y - 20 };
                AddDamageText(&damageList, punch.damage, textPos);
            }
            punch.active = false;
        }

        if (showSaveMessage) {
            messageTimer -= GetFrameTime();
            if (messageTimer <= 0) showSaveMessage = false;
        }
        
        // ===============================================
        // LÓGICA DE JOGO E COMBATE
        // ===============================================
        if (!gameOver) 
        {
            // SOCO (P2)
            if (!p2.isDead && IsKeyPressed(KEY_K) && p2.state != STATE_ATTACK) {
                p2.state = STATE_ATTACK; p2.stateTimer = 0; p2.frame = 0; p2.animTime = 0;
                punch.active = true; punch.timer = 0; punch.duration = 0.15f; punch.damage = 15;
                punch.knockback = (Vector2){ p2.direction * 300, -50 };
                punch.rect = (Rectangle){ p2.position.x + (p2.direction == 1 ? 40 : -20), p2.position.y + 35, 40, 40 };
                punch.owner = &p2; 
            }

            // PROJETIL (P1)
            if (!p1.isDead && IsKeyPressed(KEY_R) && !bullet.active && p1.state != STATE_ATTACK) {
                p1.state = STATE_ATTACK; p1.stateTimer = 0; p1.frame = 0; p1.animTime = 0;
                bullet.active = true; bullet.timer = 0; bullet.lifetime = 1.0f; bullet.damage = 10;
                bullet.velocity = (Vector2){ p1.direction * 400, 0 };
                bullet.knockback = (Vector2){ p1.direction * 200, -30 };
                bullet.rect = (Rectangle){ p1.position.x + (p1.direction == 1 ? 44 : -20), p1.position.y + 25, 20, 20 };
            }
           
            if (punch.active)
            {
                for (Enemy *e = enemies; e != NULL; e = e->next)
                {
                    if (!e->active) continue;
                    Animation *anim = &e->sprite;
                    int fw = anim->texture.width  / anim->cols;
                    int fh = anim->texture.height / anim->rows;
                    Rectangle er = { e->position.x, e->position.y, fw, fh };

                    if (CheckCollisionRecs(punch.rect, er))
                    {
                        e->hp      -= punch.damage;
                        e->velocity = (Vector2){ punch.owner->direction * 200, -30 };
                        e->isHit    = true;
                        e->hitTimer = 0;
                        if (e->hp <= 0) { e->hp = 0; e->state = ENEMY_STATE_DEAD; e->active = false; }
                        AddDamageText(&damageList, punch.damage,
                            (Vector2){ e->position.x + 10, e->position.y - 20 });
                        punch.active = false;
                        break;
                    }
                }
            }
            if (bullet.active)
            {
                for (Enemy *e = enemies; e != NULL; e = e->next)
                {
                    if (!e->active) continue;
                    Animation *anim = &e->sprite;
                    int fw = anim->texture.width  / anim->cols;
                    int fh = anim->texture.height / anim->rows;
                    Rectangle er = { e->position.x, e->position.y, fw, fh };

                    if (CheckCollisionRecs(bullet.rect, er))
                    {
                        e->hp      -= bullet.damage;
                        e->velocity = (Vector2){ p1.direction * 150, -20 };
                        e->isHit    = true;
                        e->hitTimer = 0;
                        if (e->hp <= 0) { e->hp = 0; e->state = ENEMY_STATE_DEAD; e->active = false; }
                        AddDamageText(&damageList, bullet.damage,
                            (Vector2){ e->position.x + 10, e->position.y - 20 });
                        bullet.active = false;
                        break;
                    }
                }
            }
            UpdatePlayer(&p1, speed, walls, 4, animSpeed);
            UpdatePlayer(&p2, speed, walls, 4, animSpeed);
            UpdateHitbox(&punch);
            UpdateProjectile(&bullet);

            // Verifica Mortes
            if (p1.hp <= 0 && !p1.isDead) {
                p1.isDead   = true;
                p1.hp       = 0;
                p1.state    = STATE_DEAD;
                p1.velocity = (Vector2){0, 0};
                p1.isHit    = false;
            }
            if (p2.hp <= 0 && !p2.isDead) {
                p2.isDead   = true;
                p2.hp       = 0;
                p2.state    = STATE_DEAD;
                p2.velocity = (Vector2){0, 0};
                p2.isHit    = false;
            }
            if (p1.isDead && p2.isDead) {
                gameOver = true;
                winner   = 0;
            }
        }
        else 
        {
            // TELA DE GAME OVER
            if (IsKeyPressed(KEY_SPACE)) {
                p1.hp = p1.maxHp; p1.position = (Vector2){400.0f, 300.0f};
                p1.state = STATE_SPRITE; p1.isHit = false; p1.isDead = false;
                p2.hp = p2.maxHp; p2.position = (Vector2){200.0f, 300.0f};
                p2.state = STATE_SPRITE; p2.isHit = false; p2.isDead = false;
                punch.active = false; bullet.active = false;
                FreeDamageTexts(&damageList);
                gameOver = false; winner = 0;

                currentFase = 1;
                bg = LoadTexture("assets/cenarios/Cenario1_2.0.png");
                FreeEnemies(&enemies);
                FreeEnemyBullets(&enemyBullets);
                enemies      = NULL;
                enemyBullets = NULL;
                InitWaves(&ws, 2);
                SpawnWave(&ws, &enemies,enemy1Sprite, enemy1Atk, enemy2Sprite, enemy2Atk, currentFase);
            }
        }
        
        // ===============================================
        // RENDER E DESENHO
        // ===============================================
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(bg, (Rectangle){0,0,bg.width,bg.height}, (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()}, (Vector2){0,0},0,WHITE);

        DrawPlayersSorted(&p1, &p2);
        DrawEnemies(enemies);
        DrawEnemyBullets(enemyBullets);
        DrawWaveInfo(ws);
        DrawHitbox(punch, p2.direction);
        DrawProjectile(bullet, p1.direction);

        DrawHealthBar(20, 30, p1.hp, p1.maxHp, GREEN, "GUMZ (P1)");
        DrawHealthBar(480, 30, p2.hp, p2.maxHp, BLUE, "BUBBLES (P2)");
        
        UpdateAndDrawDamageTexts(&damageList);


        if (showSaveMessage && historyList != NULL) {
            const char* currentSaveName = saveNames[(historyList->id - 1) % 10]; 
            DrawText(TextFormat("%s Salvo/Carregado!", currentSaveName), 300, 100, 20, DARKBLUE);
        }

        // Score na tela
        DrawText(TextFormat("Top Score: %s - %d pts", topNames[0], topScores[0]), 300, 560, 20, WHITE);

        if (gameOver) {
            DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.7f));
            DrawText("GAME OVER", 280, 250, 40, RED);
            DrawText("Pressione SPACE para jogar novamente", 200, 330, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    // Limpeza de Memória
    SaveScores();
    FreeDamageTexts(&damageList);
    FreeEnemies(&enemies);
    FreeEnemyBullets(&enemyBullets);
    FreeSaves(&historyList);
    FreeCutscene(&cutscene);
    
    CloseWindow();
    return 0;
}