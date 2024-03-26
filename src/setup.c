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

int32_t set_monitor_and_fps(int32_t monitor) {
	SetWindowMonitor(monitor - 1);
	const int32_t initial_fps = GetMonitorRefreshRate(monitor - 1) + 10;
	SetTargetFPS(initial_fps);
	return initial_fps;
}

static uint32_t xorshift_state = 123456789;

// XORShift random number generator function
uint32_t xorshift() {
	uint32_t x = xorshift_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	xorshift_state = x;
	return x;
}

// Faster random number generator function
float fastRand() {
	return (float)xorshift() / (float)UINT32_MAX;
}

#pragma GCC diagnostic ignored "-Wconversion"
Color rand_color_mat(uint32_t material) {
	Color temp_color = NOCOLOR;
	int32_t randR,randG,randB = 0;
	randR = (rand() % 40) - 20;
	randG = (rand() % 40) - 20;
	randB = (rand() % 40) - 20;
	switch (material) {
		case Sand:
			randR = (rand() % 40) - 20;
			randG = (rand() % 40) - 20;
			randB = (rand() % 30) - 15;
			temp_color = (Color){201 + randR, 170 + randG, 127 + randB, 255};  // 201,170,127,255
		break;
		case Water:
			randB = (rand() % 100);
			temp_color = (Color){0,0,155 + randB, 255}; // 0,0,255,255
		break;
		case Stone:
			randR = (rand() % 40) - 20;
			randG = (rand() % 40) - 20;
			randB = (rand() % 30) - 15;
			temp_color = (Color){51 + randR, 83 + randG, 69 + randB, 255}; // 51,83,69,255
		break;
		case Steam:
			randR = (rand() % 20) - 10;
			randG = (rand() % 20) - 10;
			randB = (rand() % 20) - 10;
			temp_color = (Color){42 + randR, 71 + randG, 94 + randB, 255};
		break;
		default:
			temp_color = (Color){0,0,0,0};
		break;
	}
	return temp_color;
}
#pragma GCC diagnostic warning "-Wconversion"

// Function to spawn sand brush
void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY,
					int32_t brushSize, uint32_t material, bool brushMode)
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
			float distance = (float)sqrt(pow(i - centerX, 2) + pow(j - centerY, 2));

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
				grid[i][j].color = NOCOLOR;
			}

			if (i >= 0 && i < COLS && j >= 0 && j < ROWS &&
				grid[i][j].material == Empty)
			{
				if (brushMode)
				{
					if (brushSize == 1 ||
						((float)rand() / (float)RAND_MAX > 0.8f && distance <= brushSize / 2))
					/* (fastRand() > 0.1f && distance <= brushSize / 2)) */ // Faster but looks ugly 
					{
						// Set material and properties
						grid[i][j].material = material;
						if (material == Sand)
						{
							grid[i][j].friction = 0.95f;
							grid[i][j].color = rand_color_mat(Sand);
						}
						else if (material == Water)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
							grid[i][j].color = rand_color_mat(Water);
						}
						else if (material == Stone)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 0.0f;
							grid[i][j].color = rand_color_mat(Stone);
						}
						else if (material == Steam)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
							grid[i][j].color = rand_color_mat(Steam);
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
							grid[i][j].color = rand_color_mat(Sand);
						}
						else if (material == Water)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
							grid[i][j].color = rand_color_mat(Water);
						}
						else if (material == Stone)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 0.0f;
							grid[i][j].color = rand_color_mat(Stone);
						}
					}
				}
			}
		}
	}
}

void sand(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS])
{
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
					grid[i][j + 1].color = grid[i][j].color;
					grid[i][j].color = NOCOLOR;

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
					grid[i][j + 1].color = grid_duplicate[i][j].color;
					grid[i][j].color = grid_duplicate[i][j+1].color;

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
					float randomValue = fastRand();
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
								grid[newX][j + 1].color = grid[i][j].color;
								grid[i][j].color = NOCOLOR;
							}
						}
					}
				}
			}
		}
	}
}


void updateWater(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS])
{
	for (int j = ROWS - 2; j >= 0; j--)
	{
		// Shuffle the order in which cells are processed within each row
		int order[COLS];
		for (int i = 0; i < COLS; i++)
		{
			order[i] = i;
		}

		// Shuffle the order array using Fisher-Yates shuffle algorithm
		for (int i = COLS - 1; i > 0; i--)
		{
			int randomIndex = rand() % (i + 1);
			int temp = order[i];
			order[i] = order[randomIndex];
			order[randomIndex] = temp;
		}

		// Process cells in the shuffled order
		for (int i = 0; i < COLS; i++)
		{
			int index = order[i];
			grid[index][j].spreadFactor = 5.0f;

			if (grid_duplicate[index][j].material == Water)
			{
				if (j + 1 < ROWS && grid_duplicate[index][j + 1].material == Empty && grid[index][j + 1].material == Empty)
				{
					if (j + (int32_t)grid_duplicate[index][j].spreadFactor < ROWS)
					{
						// Determine the maximum distance to spread downwards
						int32_t maxSpread = (int32_t)grid_duplicate[index][j].spreadFactor;

						// Initialize a variable to track the position to move
						int32_t targetY = j;

						// Check for empty space below the water tile within the spread distance
						for (int32_t k = 1; k <= maxSpread; k++)
						{
							// Calculate the y-coordinate of the potential destination
							int32_t newY = j + k;

							// Check if the potential destination is within bounds and is empty
							if (newY < ROWS && grid[index][newY].material == Empty && grid_duplicate[index][newY].material == Empty)
							{
								// Update the target position to move downwards
								targetY = newY;
							}
							else
							{
								// Stop spreading downwards if an obstacle is encountered
								break;
							}
						}

						// Move water to the determined target position 
						if (targetY > j && grid_duplicate[index][targetY].material == Empty && grid[index][targetY].material == Empty)
						{
							// Move water from the current position to the target position
							grid[index][targetY].material = Water;
							grid[index][targetY].color = grid[index][j].color;

							// Clear the current position
							grid[index][j].material = Empty;
							grid[index][j].color = NOCOLOR;
						}
					} 
					else if (j + 1 < ROWS && grid[index][j+1].material == Empty && grid_duplicate[index][j+1].material == Empty && grid[index][j].material == Water)
					{
						grid[index][j+1].material = Water;
						grid[index][j+1].color = grid[index][j].color;
						// Clear the current position
						grid[index][j].material = Empty;
						grid[index][j].color = NOCOLOR;

					}


					// Apply downward movement
					if (grid[index][j + 1].velocityY > 9.8f)
					{
						grid[index][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[index][j + 1].velocityY += 1.0f;
					}
				}
				else
				{
					float randomValue = fastRand();
					int direction = (randomValue <= 0.5f ? -1 : 1);
					int newX = index;

					for (int32_t k = 0; k < abs((int32_t)grid[index][j].spreadFactor); k++)
					{
						int nextX = newX + direction;
						if ((j + 1 < ROWS || grid_duplicate[nextX][j + 1].material == Empty) && nextX < COLS && nextX > 0 &&
							(j + 1 >= ROWS || grid[nextX][j + 1].material == Empty) && grid_duplicate[nextX][j].material == Empty &&
							(j >= ROWS || grid[nextX][j].material == Empty))
						{
							grid[nextX][j + 1].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[nextX][j + 1].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;
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

					if (newX >= 0 && newX < COLS && j + 1 < ROWS && grid[newX][j].material == Empty && grid_duplicate[newX][j].material == Empty)
					{
						if (grid_duplicate[newX][j].material == Empty && grid[newX][j].material == Empty)
						{
							grid[newX][j].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[newX][j].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;
						}
					}
				}
			}
		}
	}
}

void updateSteam(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS])
{
	for (int j = ROWS - 1; j > 0; j--)
	{
		// Shuffle the order in which cells are processed within each row
		int order[COLS];
		for (int i = 0; i < COLS; i++)
		{
			order[i] = i;
		}

		// Shuffle the order array using Fisher-Yates shuffle algorithm
		for (int i = COLS - 1; i > 0; i--)
		{
			int randomIndex = rand() % (i + 1);
			int temp = order[i];
			order[i] = order[randomIndex];
			order[randomIndex] = temp;
		}

		// Process cells in the shuffled order
		for (int i = 0; i < COLS; i++)
		{
			int index = order[i];
			if (grid_duplicate[index][j].material == Steam)
			{
				if (grid_duplicate[index][j - 1].material == Empty)
				{
					grid[index][j - 1].material = grid[index][j].material;
					grid[index][j].material = Empty;
					grid[index][j - 1].color = grid[index][j].color;
					grid[index][j].color = NOCOLOR;

					/* if (grid[i][j + 1].velocityY + 1.0f > 9.8f)
					{
						grid[i][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[i][j + 1].velocityY += 1.0f;
					} */
				}
				else if (grid_duplicate[index][j - 1].material == Water)
				{
					// Apply downward movement
					grid[index][j - 1].material = Steam;
					grid[index][j].material = Water;
					grid[index][j - 1].color = grid_duplicate[index][j].color;
					grid[index][j].color = grid_duplicate[index][j-1].color;

					// Apply gravity
					/* if (grid[i][j + 1].velocityY > 9.8f)
					{
						grid[i][j + 1].velocityY = 9.8f;
					}
					else
					{
						grid[i][j + 1].velocityY += 1.0f;
					} */
				}
				else
				{
					float randomValue = fastRand();
					int direction = (randomValue <= 0.5f ? -1 : 1);
					int newX = index;

					for (int32_t k = 0; k < abs((int32_t)grid[index][j].spreadFactor); k++)
					{
						int nextX = newX + direction;
						if ((j > 0 && nextX < COLS && nextX > 0 && grid_duplicate[nextX][j - 1].material == Empty) &&
							(j > 0 && grid[nextX][j - 1].material == Empty) && grid_duplicate[nextX][j].material == Empty &&
							(j < COLS && grid[nextX][j].material == Empty))
						{
								grid[nextX][j - 1].material = grid[index][j].material;
								grid[index][j].material = Empty;
								grid[nextX][j - 1].color = grid[index][j].color;
								grid[index][j].color = NOCOLOR;
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

					if (newX >= 0 && newX < COLS && j > 0 && j < ROWS && grid[newX][j].material == Empty && grid_duplicate[newX][j].material == Empty)
					{
						if (grid_duplicate[newX][j].material == Empty && grid[newX][j].material == Empty)
						{
							grid[newX][j].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[newX][j].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;
						}
					}
				}

			}
		}
	}
}

