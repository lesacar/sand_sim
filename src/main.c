#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "arg_handler.h"
#include "setup.h"

void spawnSandBrush(Cell grid[COLS][ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize);

int main(int argc, char **argv)
{
	// Initialize grid and random seed
	Cell grid[COLS][ROWS] = {0};
	srand(time(NULL));

	// Setup window and display settings
	setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_INFO, false);
	int32_t current_monitor = handle_arguments(argc, argv);
	if (current_monitor < 0)
	{
		exit(EXIT_FAILURE);
	}
	set_monitor_and_fps(current_monitor);

	// Variables for brush size and scroll hint message
	int32_t mscroll_brushSize = 1;
	bool scroll_hint_message = false;

	SetTargetFPS(4000);
	float cur_dt = 0;

	// Create a texture to render the grid
	RenderTexture2D gridTexture = LoadRenderTexture(COLS, ROWS);
	if (gridTexture.id == 0)
	{
		printf("Failed to create grid texture\n");
		exit(EXIT_FAILURE);
	}

	while (!WindowShouldClose())
	{
		cur_dt = GetFrameTime(); // Get frame time
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
		BeginDrawing();
		ClearBackground(BLACK);
		// Activate render texture
		BeginTextureMode(gridTexture);
		ClearBackground(BLACK);

		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				if (grid[i][j].material > 0)
				{
					DrawRectangle(i, j, 1, 1, WHITE);
				}
			}
		}

		// End drawing to render texture
		EndTextureMode();
		DrawTexturePro(gridTexture.texture, (Rectangle){0, 0, (float)gridTexture.texture.width, (float)-gridTexture.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0, WHITE);


		// Draw sand blocks using CPU
		/*for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				if (grid[i][j].material > 0)
				{
					DrawRectangle(i * BLOCK_SIZE, j * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, (Color){201, 170, 127, 255});
				}
			}
		}*/

		// Update brush size with mouse wheel input
		mscroll_brushSize += GetMouseWheelMove();
		if (mscroll_brushSize < 1)
			mscroll_brushSize = 1;
		else if (mscroll_brushSize > 50)
			mscroll_brushSize = 50;

		// Count sand blocks for display
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

		// Display FPS and sand block count
		DrawText(TextFormat("FPS: %.2f | Sand Blocks: %d", 1.0 / cur_dt, sandBlockCount), 20, 20, 20, RED);
		DrawText(TextFormat("Brush size: %d", mscroll_brushSize), 20, 44, 20, RED);

		// Display scroll hint message
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

		// Reset simulation if R key is pressed
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
	UnloadRenderTexture(gridTexture);
	return 0;
}
