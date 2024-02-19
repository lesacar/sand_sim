#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include "arg_handler.h"
#include "setup.h"

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720

// Define the size of the sand block in pixels
#define BLOCK_SIZE 4

// Calculate the number of rows and columns based on the screen size and block size
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)

typedef struct Cell
{
    int16_t i;
    int16_t j;
    uint8_t material;
} Cell;

// Function prototype
void spawnSandBrush(Cell grid[COLS][ROWS], int mouseX, int mouseY, int brushSize);


int32_t main(int32_t argc, char **argv)
{
    Cell grid[COLS][ROWS];
    srand(time(0));
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j].i = i;
            grid[i][j].j = j;
            grid[i][j].material = 0;
        }
    }

    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_ERROR, false);
    int32_t current_monitor = handle_arguments(argc, argv);
    if (current_monitor < 0)
    {
        exit(EXIT_FAILURE);
    }
    set_monitor_and_fps(current_monitor);

    float cur_dt = 0.0f;
    float last_dt = cur_dt;

    while (!WindowShouldClose())
    {
        last_dt = cur_dt;
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
    // Generate a random number between 0 and 1
    float randomValue = GetRandomValue(0, 1);

    // Check if sand block can move diagonally
    bool canMoveLeft = (i > 0 && j + 1 < ROWS && grid[i - 1][j + 1].material == 0 && grid[i - 1][j].material == 0);
    bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS && grid[i + 1][j + 1].material == 0 && grid[i + 1][j].material == 0);

    // If not blocked on both sides, move diagonally
    if (canMoveLeft && !canMoveRight && randomValue < 0.5)
    {
        grid[i - 1][j + 1].material = grid[i][j].material;
        grid[i][j].material = 0;
    }
    else if (!canMoveLeft && canMoveRight && randomValue >= 0.5)
    {
        grid[i + 1][j + 1].material = grid[i][j].material;
        grid[i][j].material = 0;
    }
}

                }
            }
        }

        // Draw sand blocks
        for (int i = 0; i < COLS; i++)
        {
            for (int j = 0; j < ROWS; j++)
            {
                if (grid[i][j].material > 0)
                {
                    // Draw sand block with specified dimensions
                    DrawRectangle(i * BLOCK_SIZE, j * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, YELLOW);
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
        DrawText(TextFormat("FPS: %.2f | Sand Blocks: %d", (double)((float)1.0 / (float)cur_dt), sandBlockCount), 20, 20, 20, RED);

        // Spawn sand block brush if left mouse button is held down
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            int mx = GetMouseX();
            int my = GetMouseY();

            // Get the grid indices
            int gridX = mx / BLOCK_SIZE;
            int gridY = my / BLOCK_SIZE;

            // Draw sand brush if it's not already drawn and within the grid bounds
            if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
            {
                // Call the function to spawn sand blocks in a brush-like manner
                spawnSandBrush(grid, mx, my, 5);
            }
        }

        EndDrawing();
    }

    return 0;
}

// Function definition
void spawnSandBrush(Cell grid[COLS][ROWS], int mouseX, int mouseY, int brushSize) {
    // Calculate the starting position of the brush based on the mouse position
    int startX = mouseX / BLOCK_SIZE - brushSize / 2;
    int startY = mouseY / BLOCK_SIZE - brushSize / 2;

    // Ensure the brush doesn't go outside the grid boundaries
    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;
    startX = (startX > COLS - brushSize) ? COLS - brushSize : startX;
    startY = (startY > ROWS - brushSize) ? ROWS - brushSize : startY;

    // Fill random cells within the brush area with sand
    for (int i = startX; i < startX + brushSize; i++) {
        for (int j = startY; j < startY + brushSize; j++) {
            if (GetRandomValue(0,10) > 8) { // Randomly decide whether to place sand
                grid[i][j].material = 1;
            }
        }
    }
}

