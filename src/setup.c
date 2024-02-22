#include "setup.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char *WindowTitle,
					int32_t log_lvl, bool fullscreen)
{
	SetTraceLogLevel(log_lvl);
	if (fullscreen)
	{
		SetConfigFlags(FLAG_FULLSCREEN_MODE);
	}

	InitWindow(sc_wi, sc_he, WindowTitle);
	return 0;
}

int32_t set_monitor_and_fps(int32_t monitor)
{
	SetWindowMonitor(monitor - 1);
	const int32_t initial_fps = GetMonitorRefreshRate(monitor - 1);
	SetTargetFPS(initial_fps);
	return 0;
}

// Function to spawn sand brush
// Function to spawn sand brush
void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY,
					int32_t brushSize, int32_t material, bool brushMode)
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

			// Check if the cell is within the grid bounds and is empty
			if (i >= 0 && i < COLS && j >= 0 && j < ROWS && material == 0)
			{
				grid[i][j].friction = 0.0f;
				grid[i][j].isFreeFalling = false;
				grid[i][j].mass = 0;
				grid[i][j].material = 0;
				grid[i][j].spreadFactor = 0;
				grid[i][j].spreadFactor = 0.0f;
				grid[i][j].velocityX = 0.0f;
				grid[i][j].velocityY = 0.0f;
			}

			if (i >= 0 && i < COLS && j >= 0 && j < ROWS &&
				grid[i][j].material == 0)
			{
				if (brushMode)
				{
					if (brushSize == 1 ||
						((float)rand() / RAND_MAX > 0.8f && distance <= brushSize / 2))
					{
						// Set material and properties
						grid[i][j].material = material;
						if (material == 1)
						{
							grid[i][j].friction = 0.95f;
						}
						else if (material == 2)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
						}
						else if (material == 3)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 0.0f;
						}
					}
				}
				else
				{
					if (distance <= brushSize / 2)
					{
						// Set material and properties
						grid[i][j].material = material;
						if (material == 1)
						{
							grid[i][j].friction = 0.95f;
						}
						else if (material == 2)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
						}
						else if (material == 3)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 0.0f;
						}
					}
				}
			}
		}
	}
}

void sand(Cell (*grid)[ROWS])
{
	for (int j = ROWS - 2; j >= 0; j--)
	{
		for (int i = 0; i < COLS; i++)
		{
			if (grid[i][j].material == 1)
			{
				if (grid[i][j + 1].material == 0)
				{
					grid[i][j + 1].material = grid[i][j].material;
					grid[i][j].material = 0;

					if (grid[i][j + 1].velocityY > 9.8f)
					{
						grid[i][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[i][j + 1].velocityY += 1.0f;
					}
				}
				else if (grid[i][j + 1].material == 2)
				{
					// Apply downward movement
					grid[i][j + 1].material = grid[i][j].material;
					grid[i][j].material = 2;

					// Apply gravity
					if (grid[i][j + 1].velocityY > 9.8f)
					{
						grid[i][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[i][j + 1].velocityY += 1.0f;
					}
				}
				else
				{
					float randomValue = (float)rand() / (float)RAND_MAX;
					if (randomValue <= 0.5f)
					{
						randomValue = -1.0f;
					}
					else
					{
						randomValue = 1.0f;
					}

					bool canMoveLeft =
						(i > 0 && j + 1 < ROWS && grid[i - 1][j + 1].material == 0 &&
						 grid[i - 1][j].material == 0);
					bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS &&
										 grid[i + 1][j + 1].material == 0 &&
										 grid[i + 1][j].material == 0);

					if (canMoveLeft || canMoveRight)
					{
						int newX = i + (int32_t)randomValue;
						if (newX >= 0 && newX < COLS && j + 1 < ROWS &&
							grid[newX][j].material == 0)
						{
							grid[newX][j + 1].material = grid[i][j].material;
							grid[i][j].material = 0;
						}
					}
				}
			}
		}
	}
}

// water.c
bool tempWaterDebug = false;
void updateWater(Cell (*grid)[ROWS])
{
	Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
	if (grid_duplicate == NULL)
	{
		printf("Failed to malloc grid\n");
		return;
	}
	memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
	for (int j = ROWS - 2; j >= 0; j--)
	{
		for (int i = COLS - 1; i >= 0; i--)
		{
			if (grid_duplicate[i][j].material == 2)
			{
				if (grid_duplicate[i][j + 1].material == 0) // if nothing below
				{
					grid[i][j].isFreeFalling = true;
					if (grid_duplicate[i][j].spreadFactor != 0)
					{
						grid[i][j].spreadFactor = grid_duplicate[i][j].spreadFactor; // Set spread factor for horizontal movement
					}
					else
					{
						grid[i][j].spreadFactor = 20; // Set default spread factor for downward movement
					}
				}
				else
				{
					grid[i][j].isFreeFalling = false;
					// If not falling, ensure the spread factor remains unchanged
					if (!grid[i][j].isFreeFalling && grid_duplicate[i][j].spreadFactor != 0)
					{
						grid[i][j].spreadFactor = grid_duplicate[i][j].spreadFactor;
					}
				}
				if (grid_duplicate[i][j].isFreeFalling)
				{
					// Apply downward movement
					if (j + 1 < ROWS && grid_duplicate[i][j + 1].material == 0)
					{
						grid[i][j + 1].material = grid[i][j].material;
						grid[i][j + 1].isFreeFalling = true;
						grid[i][j].material = 0;
						grid[i][j].isFreeFalling = false;
						grid[i][j + 1].velocityY = grid_duplicate[i][j].velocityY + 1.0f;
						if (grid[i][j + 1].velocityY > 9.8f)
						{
							grid[i][j + 1].velocityY = 9.8f;
						}
					}
				}
				else
				{
					// Check for horizontal movement
					int direction = 0;
					if (grid_duplicate[i][j].spreadFactor < 0)
					{
						direction = -1; // Move left
					}
					else if (grid_duplicate[i][j].spreadFactor > 0)
					{
						direction = 1; // Move right
					}
					int newX = i + direction;
					if (newX >= 0 && newX < COLS && j < ROWS &&
						grid_duplicate[newX][j].material == 0)
					{
						// Move horizontally
						grid[newX][j].material = grid_duplicate[i][j].material;
						grid[newX][j].isFreeFalling = true;
						grid[i][j].material = 0;
						grid[i][j].isFreeFalling = false;
						grid[newX][j].spreadFactor = direction; // Set spreadFactor of the new cell
					}
					else
					{
						// Reverse direction if obstacle encountered or edge reached
						grid[i][j].spreadFactor *= -1; // Reverse direction
					}
				}
			}
		}
	}
	free(grid_duplicate);
}
