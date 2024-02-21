#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "arg_handler.h"
#include "setup.h"

int main(int argc, char **argv)
{
	// Initialize grid and random seed
	Cell grid[COLS][ROWS] = {0};
	for (size_t i = 0; i < COLS; i++)
	{
		for (size_t j = 0; j < ROWS; j++)
		{
			grid[i][j].material = 0;
			grid[i][j].isFreeFalling = false;
			grid[i][j].velocityX = 0.0f;
			grid[i][j].velocityY = 0.0f;
			grid[i][j].friction = 0.0f;
		}
	}

	srand((uint32_t)time(NULL));

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

	float cur_dt = 0;
	float second_counter = 0.0f;
	float fps_counter = 0.0f;
	int frame_counter = 0;
	float average_fps = 0.0f;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	// Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, defaultCodepoints, 1);
	Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, 0, 251);

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
								 // Update frame counters
		frame_counter++;
		fps_counter += cur_dt;
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
						
						if (grid[i][j+1].velocityY > 9.8f)
						{
							grid[i][j+1].velocityY = 9.8f;
						}
						else {
							grid[i][j+1].velocityY += 1.0f;
						}
						
						
					}
					else
					{
						float randomValue = (float)rand() / (float)RAND_MAX;
						bool canMoveLeft = (i > 0 && j + 1 < ROWS && grid[i - 1][j + 1].material == 0 && grid[i - 1][j].material == 0);
						bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS && grid[i + 1][j + 1].material == 0 && grid[i + 1][j].material == 0);

						if (canMoveLeft && randomValue < 0.5f)
						{
							grid[i - 1][j + 1].material = grid[i][j].material;
							grid[i][j].material = 0;
						}
						else if (canMoveRight && randomValue >= 0.5f)
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
			int j = 0;
			while (j < ROWS)
			{
				// Find the first non-empty cell
				while (j < ROWS && grid[i][j].material == 0)
				{
					j++;
				}

				if (j == ROWS)
				{
					break; // No more non-empty cells in this column
				}

				// Start of a batch
				int startJ = j;

				// Find the end of the batch
				while (j < ROWS && grid[i][j].material > 0)
				{
					j++;
				}

				// End of batch (exclusive)
				int endJ = j;

				// Draw the batched rectangle
				DrawRectangle(i, startJ, 1, endJ - startJ, (Color){201, 170, 127, 255});
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
		mscroll_brushSize += (int32_t)GetMouseWheelMove();
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
		DrawRectangle(15, 15, 580, 50, WHITE);
		/*#ifdef WIN32
		DrawText(TextFormat("Sand Blocks: %05d | Average FPS: %.2f | FPS: %05d ", sandBlockCount, (double)average_fps, (int)(1.0f / cur_dt)), 20, 20, 20, BLACK);
		#else*/
		DrawTextEx(jetmono, TextFormat("Sand Blocks: %05d | Average FPS: %.2f | FPS: %05d ", sandBlockCount, (double)average_fps, (int)(1.0f / cur_dt)), (Vector2){20, 20}, 20, 1, BLACK);
		// #endif
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
				spawnSandBrush(grid, mx, my, mscroll_brushSize, 1);
			}
		}

		EndDrawing();
		// Accumulate time for FPS calculation
		second_counter += cur_dt;

		// If one second has passed, calculate FPS
		if (second_counter >= 1.0f)
		{
			// Calculate average FPS
			average_fps = (float)frame_counter / fps_counter;

			// Reset counters
			second_counter -= 1.0f;
			fps_counter = 0.0f;
			frame_counter = 0;
		};
	}
	UnloadRenderTexture(gridTexture);
	return 0;
}
