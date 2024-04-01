#include "setup.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

const char *MaterialTypeStrings[] = {
    "Empty",
    "Sand",
    "Water",
    "Stone",
    "Steam",
	"Spawner",
	"Void",
};

void swapTile(Cell* first, Cell* second) {
	Cell temp = *second;
	*second = *first;
	*first = temp;
	return;
}

int32_t GetMouse_X_safe() {
	int ret = GetMouseX();
	int sw = GetScreenWidth();
	if (ret > sw)
	{
		return sw;
	}
	else if (ret < 0)
	{
		return 0;
	}
	return ret;
}

int32_t GetMouse_Y_safe() {
	int ret = GetMouseY();
	int sh = GetScreenHeight();
	if (ret > sh)
	{
		return sh;
	}
	else if (ret < 0)
	{
		return 0;
	}
	return ret;
}

const char* tf_str(bool test) {
	if (test) {
		return "true";
	}
	return "false";
}

const char* str_mat(uint32_t material) {
	return MaterialTypeStrings[material];
}

// Define a special ConfigData object for error state
static const ConfigData ERROR_CONFIG = { .is_cfg_read = false, .cfg_file_size = -1, .cfg_buffer = NULL, .fps = 60, .brush_size = 10, .brush_mode=true, .read_map=false, .wants_shader=false };

ConfigData parse_config_file(const char *cfg_file) {
    ConfigData config = { .is_cfg_read = false, .cfg_file_size = 0, .cfg_buffer = NULL, .fps = 60, .brush_size = 10, .brush_mode=true };
	
	FILE *cfg_f = fopen(cfg_file, "rb");
    if (cfg_f == NULL) {
        fprintf(stderr, "Error opening file %s: %s\nUsing defaults...\n", cfg_file, strerror(errno));
        return ERROR_CONFIG;
    }

    fseek(cfg_f, 0, SEEK_END);
    config.cfg_file_size = ftell(cfg_f);
    fseek(cfg_f, 0, SEEK_SET);

    if (config.cfg_file_size > MAX_CFG_SIZE) {
        fprintf(stderr, "Error: File %s size exceeds maximum buffer size of 64KB\n", cfg_file);
        fclose(cfg_f);
        return ERROR_CONFIG;
    }

    config.cfg_buffer = (char *)malloc(config.cfg_file_size + 1); // +1 for null terminator
    if (config.cfg_buffer == NULL) {
        fprintf(stderr, "Error allocating memory: %s\n", strerror(errno));
        fclose(cfg_f);
        return ERROR_CONFIG;
    }

    size_t bytes_read = fread(config.cfg_buffer, 1, config.cfg_file_size, cfg_f);
    if (bytes_read != (size_t)config.cfg_file_size) {
        fprintf(stderr, "Error reading file %s: %s\n", cfg_file, strerror(errno));
        fclose(cfg_f);
        free(config.cfg_buffer);
        return ERROR_CONFIG;
    }
    // Null-terminate the buffer
    config.cfg_buffer[config.cfg_file_size] = '\0';

    fclose(cfg_f);
    config.is_cfg_read = true;
	config.fps = 60;
    config.brush_mode = false;
    config.brush_size = 10;

    return config;
}

// Function to reload configuration file and update ConfigData struct
char *reload_config_file(const char *cfg_file, ConfigData *config) {
    free(config->cfg_buffer);
    *config = parse_config_file(cfg_file);
    return (config->is_cfg_read) ? config->cfg_buffer : NULL;
}

void parse_config_variables(const char *cfg_buffer, ConfigData *config) {
    char *token;
    char *line;
    char *saveptr;

    // Make a copy of the buffer since strtok modifies the string
    char *buffer_copy = strdup(cfg_buffer);

    // Tokenize each line of the buffer
    line = strtok_r(buffer_copy, "\n", &saveptr);
    while (line != NULL) {
        // Tokenize each variable and its value
        token = strtok(line, "=");
        if (token != NULL) {
            // Extract variable name
            char *variable = token;

            // Extract variable value
            token = strtok(NULL, "=");
            if (token != NULL) {
                char *value = token;

                // Compare variable names and update corresponding config fields
                if (strcmp(variable, "fps") == 0) {
                    config->fps = atoi(value);
                } else if (strcmp(variable, "brush_mode") == 0) {
                    config->brush_mode = (strcmp(value, "true") == 0);
                } else if (strcmp(variable, "brush_size") == 0) {
                    config->brush_size = atoi(value);
				} else if (strcmp(variable, "read_map") == 0) {
					config->read_map = (strcmp(value, "true") == 0);
				}
                else if (strcmp(variable, "wants_shader") == 0) {
					config->wants_shader = (strcmp(value, "true") == 0);
				}
				// Add more conditions for additional variables as needed
            }
        }
        // Move to the next line
        line = strtok_r(NULL, "\n", &saveptr);
    }

    // Free the copy of the buffer
    free(buffer_copy);
}


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
		case Spawner:
			temp_color = WHITE;
			break;
		case VoidTile:
			temp_color = PURPLE;
			break;
		default:
			temp_color = (Color){0,0,0,0};
		break;
	}
	return temp_color;
}

// Function to spawn sand brush
void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY,
					int32_t brushSize, uint32_t material, uint32_t w_material ,bool brushMode)
{
	if (GetMouse_X_safe() > SCREEN_WIDTH || GetMouse_Y_safe() > SCREEN_HEIGHT)
	{
		return;
	}
	if ((material == Spawner || material == VoidTile) && grid[mouseX][mouseY].material == Empty) {
		if (material == Spawner)
		{
			grid[mouseX][mouseY].material = Spawner;
			grid[mouseX][mouseY].w_material = w_material;
			grid[mouseX][mouseY].color = rand_color_mat(Spawner);
		}
		else if (material == VoidTile)
		{
			grid[mouseX][mouseY].w_material = w_material;
			grid[mouseX][mouseY].material = VoidTile;
			grid[mouseX][mouseY].color = rand_color_mat(VoidTile);
		}
	}
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
						else if (material == Steam)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
							grid[i][j].color = rand_color_mat(Steam);
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
				grid[i][j].velocityY = 5.0f;
				if (j + 1 < ROWS && grid_duplicate[i][j + 1].material == Empty && grid[i][j + 1].material == Empty)
				{
					if (j + (int32_t)grid_duplicate[i][j].spreadFactor < ROWS)
					{
						// Determine the maximum distance to spread downwards
						int32_t maxSpread = (int32_t)grid_duplicate[i][j].velocityY;

						// Initialize a variable to track the position to move
						int32_t targetY = j;

						// Check for empty space below the water tile within the spread distance
						for (int32_t k = 1; k <= maxSpread; k++)
						{
							// Calculate the y-coordinate of the potential destination
							int32_t newY = j + k;

							// Check if the potential destination is within bounds and is empty
							if (newY < ROWS && grid[i][newY].material == Empty && grid_duplicate[i][newY].material == Empty)
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
						if (targetY > j && grid_duplicate[i][targetY].material == Empty && grid[i][targetY].material == Empty)
						{
							// Move water from the current position to the target position
							/*grid[i][targetY].material = Water;
							grid[i][targetY].color = grid[i][j].color;

							// Clear the current position
							grid[i][j].material = Empty;
							grid[i][j].color = NOCOLOR;*/
							swapTile(&grid[i][j], &grid[i][targetY]);
						}
					} 
					else if (j + 1 < ROWS && grid[i][j+1].material == Empty && grid_duplicate[i][j+1].material == Empty && grid[i][j].material == Sand)
					{
						/*grid[i][j+1].material = Water;
						grid[i][j+1].color = grid[i][j].color;
						// Clear the current position
						grid[i][j].material = Empty;
						grid[i][j].color = NOCOLOR;*/
						swapTile(&grid[i][j+1], &grid[i][j]);

					}


					// Apply downward movement
				}
				/*if (grid_duplicate[i][j + 1].material == Empty)
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
					swapTile(&grid[i][j], &grid[i][j+1]);
				}*/
				else if (grid_duplicate[i][j + 1].material == Water)
				{
					// Apply downward movement
					/*grid[i][j + 1].material = Sand;
					grid[i][j].material = Water;

					grid[i][j + 1].color = grid[i][j].color;
					grid[i][j].color = grid_duplicate[i][j+1].color;*/

					// Apply gravity
					swapTile(&grid[i][j], &grid[i][j+1]);
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
						(i > 0 && j + 1 < ROWS && (grid[i - 1][j + 1].material == Empty || grid[i - 1][j + 1].material == Water) &&
						(grid[i - 1][j].material == Empty || grid[i - 1][j].material == Water));
					bool canMoveRight = (i < COLS - 1 && j + 1 < ROWS &&
						(grid[i + 1][j + 1].material == Empty || grid[i + 1][j + 1].material == Water) &&
						(grid[i + 1][j].material == Empty || grid[i + 1][j].material == Water));

					if (canMoveLeft || canMoveRight)
					{
						int newX = i + (int32_t)randomValue;
						if (newX >= 0 && newX < COLS && j + 1 < ROWS &&
							(grid[newX][j].material == Empty || grid[newX][j].material == Water ))
						{
							// Check if the target position is already occupied
							if (grid[newX][j + 1].material == Empty || grid[newX][j + 1].material == Water)
							{
								// Move the current sand particle to the new position
								/*grid[newX][j + 1].material = Sand;
								grid[i][j].material = Empty;
								grid[newX][j + 1].color = grid[i][j].color;
								grid[i][j].color = NOCOLOR;*/
								swapTile(&grid[newX][j+1], &grid[i][j]);
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

			if (grid_duplicate[index][j].material == Water)
			{
				grid[index][j].spreadFactor = 5.0f;
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
							/*grid[index][targetY].material = Water;
							grid[index][targetY].color = grid[index][j].color;

							// Clear the current position
							grid[index][j].material = Empty;
							grid[index][j].color = NOCOLOR;*/
							swapTile(&grid[index][targetY],&grid[index][j]);
						}
					} 
					else if (j + 1 < ROWS && grid[index][j+1].material == Empty && grid_duplicate[index][j+1].material == Empty && grid[index][j].material == Water)
					{
						/*grid[index][j+1].material = Water;
						grid[index][j+1].color = grid[index][j].color;
						// Clear the current position
						grid[index][j].material = Empty;
						grid[index][j].color = NOCOLOR;*/
						swapTile(&grid[index][j+1],&grid[index][j]);

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
							/*
							grid[nextX][j + 1].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[nextX][j + 1].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;*/
							swapTile(&grid[nextX][j+1],&grid[index][j]);
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
							swapTile(&grid[newX][j],&grid[index][j]);
							/*
							grid[newX][j].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[newX][j].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;*/
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
				grid[index][j].spreadFactor = 5.0f;
				if (grid_duplicate[index][j - 1].material == Empty || grid_duplicate[index][j - 1].material == Water)
				{
					/*grid[index][j - 1].material = grid[index][j].material;
					grid[index][j].material = Empty;
					grid[index][j - 1].color = grid[index][j].color;
					grid[index][j].color = NOCOLOR;*/
					// Apply downward movement
					/*grid[index][j - 1].material = Steam;
					grid[index][j].material = Water;
					Color tempcolor = grid[index][j-1].color;
					grid[index][j - 1].color = grid[index][j].color;
					grid[index][j].color = tempcolor;*/
					swapTile(&grid[index][j-1],&grid[index][j]);
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
								/*grid[nextX][j - 1].material = grid[index][j].material;
								grid[index][j].material = Empty;
								grid[nextX][j - 1].color = grid[index][j].color;
								grid[index][j].color = NOCOLOR;*/
								swapTile(&grid[index][j], &grid[nextX][j]);
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
							/*grid[newX][j].material = grid[index][j].material;
							grid[index][j].material = Empty;
							grid[newX][j].color = grid[index][j].color;
							grid[index][j].color = NOCOLOR;*/
							swapTile(&grid[index][j], &grid[newX][j]);
						}
					}
				}

			}
		}
	}
}

