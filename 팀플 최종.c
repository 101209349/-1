#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 10
#define BLOCK_WIDTH 60
#define BLOCK_HEIGHT 20
#define BLOCK_COLS 12
#define BLOCK_ROWS 3
#define MAX_BLOCKS (BLOCK_COLS * BLOCK_ROWS)

typedef struct {
    Vector2 position;
    Vector2 speed;
} Ball;

typedef struct {
    Rectangle rect;
    Color color;
} Block;

typedef struct {
    int score;
    int lives;
} GameState;

void DrawStartScreen() {
    ClearBackground(RAYWHITE);
    DrawText("Breakout Game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 30, BLACK);
    DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 10, 20, BLACK);
}

void DrawGameScreen(GameState* gameState, Ball* ball, Rectangle* paddle, Block blocks[MAX_BLOCKS]) {
    ClearBackground(RAYWHITE);

    // Draw Score and Lives
    DrawText(TextFormat("Score: %i", gameState->score), 10, 10, 20, BLACK);
    DrawText(TextFormat("Lives: %i", gameState->lives), SCREEN_WIDTH - 100, 10, 20, BLACK);

    // Draw Ball
    DrawCircleV(ball->position, BALL_RADIUS, BLUE);

    // Draw Paddle
    DrawRectangleRec(*paddle, DARKBLUE);

    // Draw Blocks
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].rect.width > 0) {  // If block is not destroyed
            DrawRectangleRec(blocks[i].rect, blocks[i].color);
        }
    }
}

void UpdateBall(Ball* ball, Rectangle* paddle, Block blocks[MAX_BLOCKS], GameState* gameState) {
    // Move Ball
    ball->position.x += ball->speed.x;
    ball->position.y += ball->speed.y;

    // Ball-Wall Collision (top, left, right)
    if (ball->position.x - BALL_RADIUS < 0 || ball->position.x + BALL_RADIUS > SCREEN_WIDTH) {
        ball->speed.x *= -1;  // Reverse X direction
    }
    if (ball->position.y - BALL_RADIUS < 0) {
        ball->speed.y *= -1;  // Reverse Y direction
    }

    // Ball-Paddle Collision
    if (CheckCollisionCircleRec(ball->position, BALL_RADIUS, *paddle)) {
        ball->speed.y *= -1;  // Reverse Y direction
        ball->position.y = paddle->y - BALL_RADIUS;  // Place ball on top of paddle
    }

    // Ball Block Collision
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].rect.width > 0 && CheckCollisionCircleRec(ball->position, BALL_RADIUS, blocks[i].rect)) {
            ball->speed.y *= -1;  // Reverse Y direction
            blocks[i].rect.width = 0;  // Destroy block
            gameState->score += 10;  // Increase score
        }
    }

    // Ball falls below the screen (game over)
    if (ball->position.y + BALL_RADIUS > SCREEN_HEIGHT) {
        gameState->lives--;
        if (gameState->lives > 0) {
            // Reset Ball Position
            ball->position.x = SCREEN_WIDTH / 2;
            ball->position.y = SCREEN_HEIGHT / 2;
            ball->speed.x = 5.0f;
            ball->speed.y = -5.0f;
        }
        else {
            gameState->score = 0;
        }
    }
}

void UpdatePaddle(Rectangle* paddle) {
    // Paddle Movement (keyboard control)
    if (IsKeyDown(KEY_LEFT) && paddle->x > 0) {
        paddle->x -= 10;
    }
    if (IsKeyDown(KEY_RIGHT) && paddle->x + PADDLE_WIDTH < SCREEN_WIDTH) {
        paddle->x += 10;
    }

    // Paddle Movement (mouse control)
    paddle->x = GetMouseX() - PADDLE_WIDTH / 2;
    if (paddle->x < 0) paddle->x = 0;
    if (paddle->x + PADDLE_WIDTH > SCREEN_WIDTH) paddle->x = SCREEN_WIDTH - PADDLE_WIDTH;
}

void InitializeBlocks(Block blocks[MAX_BLOCKS]) {
    // 무지개 색상 배열 (Red, Orange, Yellow, Green, Blue, Indigo, Violet)
    Color rainbowColors[] = {
        RED, ORANGE, YELLOW, GREEN, BLUE, DARKBLUE, PURPLE
    };

    int blockIndex = 0;
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            blocks[blockIndex].rect.x = col * (BLOCK_WIDTH + 5);
            blocks[blockIndex].rect.y = row * (BLOCK_HEIGHT + 5) + 50;
            blocks[blockIndex].rect.width = BLOCK_WIDTH;
            blocks[blockIndex].rect.height = BLOCK_HEIGHT;

            // 색상을 무지개 색으로 지정 (각 블록의 색상을 순차적으로 설정)
            blocks[blockIndex].color = rainbowColors[(blockIndex % 7)];

            blockIndex++;
        }
    }
}

int main(void) {
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");

    // Game State
    GameState gameState = { 0, 3 };

    // Paddle and Ball
    Rectangle paddle = { SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIGHT - 10, PADDLE_WIDTH, PADDLE_HEIGHT };
    Ball ball = { { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, { 5.0f, -5.0f } };

    // Blocks
    Block blocks[MAX_BLOCKS];
    InitializeBlocks(blocks);

    SetTargetFPS(60);

    bool inMainMenu = true;

    // Main game loop
    while (!WindowShouldClose()) {
        if (inMainMenu) {
            DrawStartScreen();
            if (IsKeyPressed(KEY_ENTER)) {
                inMainMenu = false;  // Start game
            }
        }
        else {
            // Update game logic
            UpdateBall(&ball, &paddle, blocks, &gameState);
            UpdatePaddle(&paddle);

            // Check game over condition
            if (gameState.lives <= 0 || gameState.score == MAX_BLOCKS * 10) {
                inMainMenu = true;  // Go back to main menu
                gameState.lives = 3;
                ball = { { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, { 5.0f, -5.0f } };
                InitializeBlocks(blocks);  // Reset blocks
            }

            // Draw game screen
            DrawGameScreen(&gameState, &ball, &paddle, blocks);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();  // Close window and OpenGL context

    return 0;
}
