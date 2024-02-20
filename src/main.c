#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "arg_handler.h"
#include "setup.h"

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720
#define BLOCK_SIZE 4
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)

typedef struct Cell
{
    uint8_t material;
} Cell;

void spawnSandBrush(Cell grid[COLS][ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize);

int main(int argc, char **argv)
{
    Cell grid[COLS][ROWS] = {0};
    srand(time(NULL));

    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_ERROR, false);
    int32_t current_monitor = handle_arguments(argc, argv);
    if (current_monitor < 0)
    {
        exit(EXIT_FAILURE);
    }
    set_monitor_and_fps(current_monitor);

    int32_t mscroll_brushSize = 1;
    bool scroll_hint_message = false;

    SetTargetFPS(4000);
	float cur_dt = 0;
    while (!WindowShouldClose())
    {
		cur_dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);

        // Update falling logic for sand blocks
        for (int j = ROWS - 2; j >= 0; j--)
        {
            for (int i = 0; i < COLS; i++)
            {
                if (grid[i][j].material > 0)
                {
                    if (grid[i][j + 1].material == 0)
                    {
                        grid[i][j + 1].material = grid[i][j].material;
                        grid[i][j].material = 0;
                    }
                    else
                    {
                        float randomValue = (float)rand() / RAND_MAX;
                        bool canMoveLeft = (i > 0 && j + 1 < ROWS && grid[i - 1][j + 1].material == 0 && grid[i - 1][j].material == 0);
                        bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS && grid[i + 1][j + 1].material == 0 && grid[i + 1][j].material == 0);

                        if (canMoveLeft && !canMoveRight && randomValue < 0.5f)
                        {
                            grid[i - 1][j + 1].material = grid[i][j].material;
                            grid[i][j].material = 0;
                        }
                        else if (!canMoveLeft && canMoveRight && randomValue >= 0.5f)
                        {
                            grid[i + 1][j + 1].material = grid[i][j].material;
                            grid[i][j].material = 0;
                        }
                    }
                }
            }
        }

        mscroll_brushSize += GetMouseWheelMove();
        if (mscroll_brushSize < 1)
            mscroll_brushSize = 1;
        else if (mscroll_brushSize > 50)
            mscroll_brushSize = 50;

        // Draw sand blocks
        for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                if (grid[i][j].material > 0)
                {
                    DrawRectangle(i * BLOCK_SIZE, j * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, (Color){201, 170, 127, 255});
                }
            }
        }

        int sandBlockCount = 0;
        for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                if (grid[i][j].material > 0)
                {
                    sandBlockCount++;
                }
            }
        }

        DrawText(TextFormat("FPS: %.2f | Sand Blocks: %d", 1.0/cur_dt, sandBlockCount), 20, 20, 20, RED);
        DrawText(TextFormat("Brush size: %d", mscroll_brushSize), 20, 44, 20, RED);

        if (!scroll_hint_message)
        {
            DrawText("Scroll with mouse wheel to increase/reduce brush size (max 50)", 20, 66, 20, YELLOW);
            DrawText("Press R to reset simulation", 20, 88, 20, YELLOW);
            DrawText("Press H to hide this message", 20, 110, 20, YELLOW);
            if (IsKeyPressed(KEY_H))
            {
                scroll_hint_message = true;
            }
        }

        if (IsKeyPressed(KEY_R))
        {
            memset(grid, 0, sizeof(grid));
        }

        // Spawn sand block brush if left mouse button is held down
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            int mx = GetMouseX();
            int my = GetMouseY();

            int gridX = mx / BLOCK_SIZE;
            int gridY = my / BLOCK_SIZE;

            if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
            {
                spawnSandBrush(grid, mx, my, mscroll_brushSize);
            }
        }

        EndDrawing();
    }

    return 0;
}

void spawnSandBrush(Cell grid[COLS][ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize)
{
    int startX = mouseX / BLOCK_SIZE - brushSize / 2;
    int startY = mouseY / BLOCK_SIZE - brushSize / 2;

    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;
    startX = (startX > COLS - brushSize) ? COLS - brushSize : startX;
    startY = (startY > ROWS - brushSize) ? ROWS - brushSize : startY;

    int centerX = startX + brushSize / 2;
    int centerY = startY + brushSize / 2;

    for (int i = startX; i < startX + brushSize; i++)
    {
        for (int j = startY; j < startY + brushSize; j++)
        {
            float distance = sqrt(pow(i - centerX, 2) + pow(j - centerY, 2));

            if (brushSize == 1)
            {
                grid[i][j].material = 1;
            }
            else if ((float)rand() / RAND_MAX > 0.8f && distance <= brushSize / 2)
            {
                grid[i][j].material = 1;
            }
        }
    }
}
