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

    int controls; // 0 = WASD, 1 = SETAS
} Player;

void UpdatePlayer(Player *p, float speed, Rectangle *walls, int wallCount, float animSpeed)
{
    Vector2 next = p->position;

    // ================= CONTROLES =================
    if (p->controls == 0) {
        if (IsKeyDown(KEY_D)) { next.x += speed; p->direction = 1; }
        if (IsKeyDown(KEY_A)) { next.x -= speed; p->direction = -1; }
        if (IsKeyDown(KEY_W)) next.y -= speed;
        if (IsKeyDown(KEY_S)) next.y += speed;
    } else {
        if (IsKeyDown(KEY_RIGHT)) { next.x += speed; p->direction = 1; }
        if (IsKeyDown(KEY_LEFT)) { next.x -= speed; p->direction = -1; }
        if (IsKeyDown(KEY_UP)) next.y -= speed;
        if (IsKeyDown(KEY_DOWN)) next.y += speed;
    }

    // ================= COLISÃO =================
    int frameW = p->sprite.width / p->cols;
    int frameH = p->sprite.height / p->rows;

    Rectangle rectX = { next.x, p->position.y, frameW, frameH };
    Rectangle rectY = { p->position.x, next.y, frameW, frameH };

    bool colX = false;
    bool colY = false;

    for (int i = 0; i < wallCount; i++) {
        if (CheckCollisionRecs(rectX, walls[i])) colX = true;
        if (CheckCollisionRecs(rectY, walls[i])) colY = true;
    }

    if (!colX) p->position.x = next.x;
    if (!colY) p->position.y = next.y;

    // ================= ANIMAÇÃO =================
    p->animTime += GetFrameTime();

    if (p->animTime >= animSpeed) {
        p->animTime = 0;
        p->frame++;

        if (p->frame >= p->totalFrames)
            p->frame = 0;
    }
}

void DrawPlayer(Player p)
{
    int frameW = p.sprite.width / p.cols;
    int frameH = p.sprite.height / p.rows;

    int row = p.frame / p.cols;
    int col = p.frame % p.cols;

    Rectangle source = {
        col * frameW,
        row * frameH,
        frameW,
        frameH
    };

    if (p.direction == -1) {
        source.x += frameW;
        source.width = -frameW;
    }

    Rectangle dest = {
        p.position.x,
        p.position.y,
        frameW,
        frameH
    };

    DrawTexturePro(p.sprite, source, dest, (Vector2){0,0}, 0, WHITE);
}

void DrawPlayersSorted(Player *p1, Player *p2)
{
    Player *first;
    Player *second;

    if (p1->position.y < p2->position.y) {
        first = p1;
        second = p2;
    } else {
        first = p2;
        second = p1;
    }

    DrawPlayer(*first);
    DrawPlayer(*second);
}

int main()
{
    InitWindow(800, 600, "Beat'em Up Diferente");

    Texture2D bg = LoadTexture("cenario1.png");

    Texture2D sprite1 = LoadTexture("gumz.png");   
    Texture2D sprite2 = LoadTexture("bubble.png");  

    SetTextureFilter(sprite1, TEXTURE_FILTER_POINT);
    SetTextureFilter(sprite2, TEXTURE_FILTER_POINT);

    Player p1 = {
        .position = {400, 300},
        .direction = 1,
        .frame = 0,
        .animTime = 0,
        .sprite = sprite1,
        .cols = 1,
        .rows = 10,
        .totalFrames = 10,
        .controls = 0
    };

    Player p2 = {
        .position = {200, 300},
        .direction = 1,
        .frame = 0,
        .animTime = 0,
        .sprite = sprite2,
        .cols = 1,
        .rows = 4,
        .totalFrames = 4,
        .controls = 1
    };

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
        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

        float speed = moveSpeed * GetFrameTime();

        // ================= UPDATE =================
        UpdatePlayer(&p1, speed, walls, 4, animSpeed);
        UpdatePlayer(&p2, speed, walls, 4, animSpeed);

        // ================= DRAW =================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Rectangle srcBg = {0, 0, bg.width, bg.height};
        Rectangle dstBg = {0, 0, GetScreenWidth(), GetScreenHeight()};
        DrawTexturePro(bg, srcBg, dstBg, (Vector2){0,0}, 0, WHITE);

        // teste das paredes (vermelhas)
        for (int i = 0; i < 4; i++)
            DrawRectangleLinesEx(walls[i], 2, RED);

        
            DrawPlayersSorted(&p1, &p2);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
