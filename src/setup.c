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
			if (i >= 0 && i < COLS && j >= 0 && j < ROWS && material == Empty)
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
						if (material == Sand)
						{
							grid[i][j].friction = 0.95f;
						}
						else if (material == Water)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
						}
						else if (material == Stone)
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
						if (material == Sand)
						{
							grid[i][j].friction = 0.95f;
						}
						else if (material == Water)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
						}
						else if (material == Stone)
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
	Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
	if (grid_duplicate == NULL)
	{
		printf("Failed to malloc grid\n");
		return;
	}
	memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
	for (int j = ROWS - 2; j >= 0; j--)
	{
		for (int i = 0; i < COLS; i++)
		{
			if (grid_duplicate[i][j].material == Sand)
			{
				if (grid_duplicate[i][j + 1].material == Empty)
				{
					grid[i][j + 1].material = grid[i][j].material;
					grid[i][j].material = Empty;

					if (grid[i][j + 1].velocityY + 1.0f > 9.8f)
					{
						grid[i][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[i][j + 1].velocityY += 1.0f;
					}
				}
				else if (grid_duplicate[i][j + 1].material == Water)
				{
					// Apply downward movement
					grid[i][j + 1].material = Sand;
					grid[i][j].material = Water;

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
						(i > 0 && j + 1 < ROWS && grid[i - 1][j + 1].material == Empty &&
						 grid[i - 1][j].material == Empty);
					bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS &&
										 grid[i + 1][j + 1].material == Empty &&
										 grid[i + 1][j].material == Empty);

					if (canMoveLeft || canMoveRight)
					{
						int newX = i + (int32_t)randomValue;
						if (newX >= 0 && newX < COLS && j + 1 < ROWS &&
							grid[newX][j].material == Empty)
						{
							// Check if the target position is already occupied
							if (grid[newX][j + 1].material == Empty)
							{
								// Move the current sand particle to the new position
								grid[newX][j + 1].material = Sand;
								grid[i][j].material = Empty;
							}
						}
					}
				}
			}
		}
	}
	free(grid_duplicate);
}

void updateWater(Cell (*grid)[ROWS])
{
	int apf = 3;
	int apfC = 0;
	Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
	if (grid_duplicate == NULL)
	{
		printf("Failed to malloc grid\n");
		return;
	}
	memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);

	for (int j = ROWS - 2; j >= 0; j--)
	{
		for (int i = 0; i < COLS; i++)
		{
			grid[i][j].spreadFactor = 5.0f;
			if (grid_duplicate[i][j].material == Water)
			{

				if (j + 1 < ROWS && grid_duplicate[i][j + 1].material == Empty && grid[i][j + 1].material == Empty)
				{
					// Apply downward movement
					grid[i][j + 1].material = grid[i][j].material;
					grid[i][j].material = Empty;

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
					int tempFactor = (int32_t)grid[i][j].spreadFactor;
					while (apfC < apf)
					{

						// Attempt horizontal movement
						float randomValue = (float)rand() / (float)RAND_MAX;
						int direction = (randomValue <= 0.5f ? -1 : 1);
						int newX = i;

						// printf("%d\n", tempFactor);
						grid[i][j].spreadFactor = (float)(rand() % tempFactor);
						for (int32_t k = 0; k < abs((int32_t)grid[i][j].spreadFactor); k++)
						{
							int nextX = newX + direction;
							if (grid_duplicate[nextX][j + 1].material == Empty && nextX + i < COLS &&
								(j + 1 >= ROWS || grid[nextX][j + 1].material == Empty) && grid_duplicate[nextX][j].material == Empty &&
								(j >= ROWS || grid[nextX][j].material == Empty))
							{
								grid[nextX][j + 1].material = grid[i][j].material;
								grid[i][j].material = Empty;
							}

							else if (nextX >= 0 && nextX < COLS && grid[nextX][j].material != Empty)
							{
								// Stop horizontal movement if an obstacle is encountered
								break;
							}
							else
							{
								// Update newX if the next position is valid and empty
								newX = nextX;
							}
						}

						// printf("%d:%d ", newX, i);

						// printf("%d:%d:%f  ", newX, i, grid[i][j].spreadFactor);
						// Check if newX is within bounds and the target cell is empty
						if (newX >= 0 && newX < COLS && j + 1 < ROWS && grid[newX][j].material == Empty && grid_duplicate[newX][j].material == Empty)
						{

							//  Check if the target cell is not already occupied by another water particle
							// if (grid[newX][j].material == Empty && grid[newX][j].material == Empty)
							if (grid_duplicate[newX][j].material == Empty && grid[newX][j].material == Empty)
							{
								grid[newX][j].material = grid[i][j].material;
								grid[i][j].material = Empty;
							}
						}
						++apfC;
					}
					apfC = 0;
				}
			}
		}
	}
	free(grid_duplicate);
}
