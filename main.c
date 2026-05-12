
#include <raylib.h>

int main() {
    ToggleFullscreen();
    InitWindow(800, 600, "jogo");
    
   	
    Texture2D sprite = LoadTexture("gumz.png");
    SetTextureFilter(sprite, TEXTURE_FILTER_POINT);

    int cols = 1;
    int rows = 10;
    int totalFrames = 10;

    int currentFrame = 0;

    int frameWidth = sprite.width / cols;
    int frameHeight = sprite.height / rows;

    float animTime = 0.0f;
    float animSpeed = 0.1f;

    Vector2 position = {400, 300};
    float moveSpeed = 200.0f;

    int direction = 1;

    while (!WindowShouldClose()) {
	// ================TELA CHEIA=================
	
	if (IsKeyPressed(KEY_F11)){
		ToggleFullscreen();
	}

        // ================= MOVIMENTO =================
        bool isMoving = false;

        float speed = moveSpeed * GetFrameTime();

        if (IsKeyDown(KEY_D)) {
            position.x += speed;
            direction = 1;
            isMoving = true;
        }

        if (IsKeyDown(KEY_A)) {
            position.x -= speed;
            direction = -1;
            isMoving = true;
        }

        if (IsKeyDown(KEY_W)) {
            position.y -= speed;
            isMoving = true;
        }

        if (IsKeyDown(KEY_S)) {
            position.y += speed;
            isMoving = true;
        }

        // ================= ANIMAÇÃO =================
        animTime += GetFrameTime();

        if (animTime >= animSpeed) {
            animTime = 0;
            currentFrame++;

            if (currentFrame >= totalFrames) {
                currentFrame = 0;
            }
        }

        // ================= FRAME =================
        int row = currentFrame / cols;
        int col = currentFrame % cols;

        Rectangle source = {
            col * frameWidth,
            row * frameHeight,
            frameWidth,
            frameHeight
        };

        // ================= FLIP CORRETO =================
        if (direction == -1) {
            source.x += frameWidth;
            source.width = -frameWidth;
        }

        Rectangle dest = {
            position.x,
            position.y,
            frameWidth,
            frameHeight
        };

        // ================= DESENHO =================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(
            sprite,
            source,
            dest,
            (Vector2){0,0},
            0,
            WHITE
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
