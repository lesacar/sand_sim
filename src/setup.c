#include "setup.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>



uint8_t Draw_message_box(MsgBox *msgbox, Font *font) {
	if (!msgbox->show) {
		return 0;
	}
	//DrawRectangleRec(msgbox->position, YELLOW);
	int txtfontsize = 20;
	DrawRectangleRounded(msgbox->position, 0.15f, 0, DARKGRAY);
	Rectangle title_top = {msgbox->position.x,msgbox->position.y,msgbox->position.width,24};
	Rectangle title_bottom = {msgbox->position.x,msgbox->position.y+12,msgbox->position.width,12};

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), title_top)) {
		msgbox->dragging = true;
		Vector2 mousePos = GetMousePosition();
		msgbox->drag_offset.x = mousePos.x - msgbox->position.x;
		msgbox->drag_offset.y = mousePos.y - msgbox->position.y;
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		msgbox->dragging = false;
	}

	if (msgbox->dragging) {
		Vector2 mousePos = GetMousePosition();
		msgbox->position.x = mousePos.x - msgbox->drag_offset.x;
		msgbox->position.y = mousePos.y - msgbox->drag_offset.y;
	}

	DrawRectangleRounded(title_top, 0.92f, 0, GRAY);
	DrawRectangleRec(title_bottom, GRAY);
	//DrawRectangleRoundedLines((Rectangle){msgbox->position.x+1,msgbox->position.y+1,msgbox->position.width-2,msgbox->position.height-2}, 0.15f, 10, 1.0f, WHITE);
	DrawTextEx(*font, msgbox->title, (Vector2){msgbox->position.x+12,msgbox->position.y}, 24, 0, WHITE);

	int32_t charsPerLine = (msgbox->position.width - 4) / MeasureTextEx(*font, "a", txtfontsize, 0).x;
for (int32_t i = 0; i < (msgbox->position.height/txtfontsize)-3; i++) {
    char txt[charsPerLine + 1];
	size_t textOffset = i * charsPerLine;
    if (textOffset >= strlen(msgbox->text)) {
        break;
    }
    int j;
    for (j = 0; j < charsPerLine && msgbox->text[textOffset + j] != '\0'; j++) {
        txt[j] = msgbox->text[textOffset + j];
    }
    txt[j] = '\0';
	DrawTextEx(*font, txt, (Vector2){msgbox->position.x + 1, msgbox->position.y + 26 + (i * (MeasureTextEx(*font, "a", txtfontsize, 0).y -2))}, txtfontsize, 0, WHITE);
}


	Rectangle ok = {0};
	Rectangle cancel = {0};
	uint8_t btnoffsetW = 35;
	uint8_t btnoffsetH = 25;
	switch (msgbox->buttons) {
		case 0:
			return 0;
		break;
		case 1:
			ok = (Rectangle){msgbox->position.x+msgbox->position.width/2-btnoffsetW,msgbox->position.y+msgbox->position.height-btnoffsetH,btnoffsetW*2,btnoffsetH-5};
			//DrawRectangleRec(ok,WHITE); // jagged
			DrawRectangleRounded((Rectangle){ok.x+1,ok.y+1,ok.width-2,ok.height-2}, 0.3f, 10, WHITE);
			//DrawRectangleLinesEx(ok, 2.0f, BLACK);
			DrawRectangleRoundedLines((Rectangle){ok.x+1,ok.y+1,ok.width-2,ok.height-2}, 0.3f, 0, 2.0f, BLACK);
			DrawTextEx(*font, "Ok", (Vector2){ok.x+ok.width/2-MeasureTextEx(*font,"Ok",btnoffsetH/1.2,0).x/2,ok.y}, btnoffsetH/1.2, 0, BLACK);
			if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), ok)) || IsKeyPressed(KEY_O)) {
				msgbox->show = false;
				return 1;
				break;
				case 2:
					ok = (Rectangle){msgbox->position.x+msgbox->position.width/2-btnoffsetW*2-(btnoffsetW/3),msgbox->position.y+msgbox->position.height-btnoffsetH,btnoffsetW*2,btnoffsetH-5};
					cancel = (Rectangle){msgbox->position.x+msgbox->position.width/2+(btnoffsetW/3),msgbox->position.y+msgbox->position.height-btnoffsetH,btnoffsetW*2,btnoffsetH-5};
					//DrawRectangleRec(ok,WHITE); // jagged
					DrawRectangleRounded(ok, 0.3f, 0, WHITE);
					DrawRectangleRounded(cancel, 0.3f, 0, WHITE);
					//DrawRectangleLinesEx(ok, 2.0f, BLACK);
					DrawRectangleRoundedLines((Rectangle){ok.x+1,ok.y+1,ok.width-2,ok.height-2}, 0.3f, 0, 2.0f, BLACK);
					DrawRectangleRoundedLines((Rectangle){cancel.x+1,cancel.y+1,cancel.width-2,cancel.height-2}, 0.3f, 0, 2.0f, BLACK);
					DrawTextEx(*font, "Ok", (Vector2){ok.x+ok.width/2-MeasureTextEx(*font,"Ok",btnoffsetH/1.2,0).x/2,ok.y}, btnoffsetH/1.2, 0, BLACK);
					DrawTextEx(*font, "Cancel", (Vector2){cancel.x+cancel.width/2-MeasureTextEx(*font,"Cancel",btnoffsetH/1.2,0).x/2,cancel.y}, btnoffsetH/1.2, 0, BLACK);
					if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), ok)) || IsKeyPressed(KEY_O)) {
						msgbox->show = false;
						return 1;
					} else if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), cancel)) || IsKeyPressed(KEY_C)) {
						msgbox->show = false;
						return 2;
					}
				break;
				default:
				break;
		}
	}
	return 0;
}

// Define a special ConfigData object for error state
static const ConfigData ERROR_CONFIG = { .is_cfg_read = false, .cfg_file_size = -1, .cfg_buffer = NULL, .fps = 60, .brush_size = 10, .brush_mode=true, .read_map=false, .wants_shader=false, .random_grid_randomness = 8 };

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
				} else if (strcmp(variable, "wants_shader") == 0) {
					config->wants_shader = (strcmp(value, "true") == 0);
				} else if (strcmp(variable, "random_grid_randomness") == 0) {
					config->random_grid_randomness = atoi(value);
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
	// SetConfigFlags(FLAG_WINDOW_RESIZABLE);

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
		case Obsidian:
			randR = (rand() % 20) - 10;
			randG = (rand() % 20) - 10;
			randB = (rand() % 20) - 10;
			temp_color = (Color){113 + randR, 98 + randG, 122 + randB, 255};
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
	    if ((material == Spawner || material == VoidTile))
    {
        // Check if a Spawner or VoidTile already exists at the target location
        if (grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].material != Empty)
        {
            return; // Return if Spawner or VoidTile already exists
        }

        // Place the Spawner or VoidTile at the target location
        if (material == Spawner)
        {
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].material = Spawner;
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].w_material = w_material;
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].color = rand_color_mat(Spawner);
            return;
        }
        else if (material == VoidTile)
        {
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].w_material = w_material;
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].material = VoidTile;
            grid[mouseX / BLOCK_SIZE][mouseY / BLOCK_SIZE].color = rand_color_mat(VoidTile);
            return;
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
						else if (material == Obsidian)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 0.0f;
							grid[i][j].color = rand_color_mat(Obsidian);
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
						else if (material == Obsidian)
						{
							grid[i][j].friction = 0.0f;
							grid[i][j].spreadFactor = 5.0f;
							grid[i][j].color = rand_color_mat(Obsidian);
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
				else if (grid_duplicate[i][j + 1].material == Water)
				{
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
						(i > 0 && j + 1 < ROWS && (grid[i - 1][j + 1].material == Empty || grid[i - 1][j + 1].material == Water || 
						grid[i - 1][j + 1].material == Steam) &&
						(grid[i - 1][j].material == Empty || grid[i - 1][j].material == Water || grid[i - 1][j].material == Steam));
					bool canMoveRight = 
						(i < COLS - 1 && j + 1 < ROWS && (grid[i + 1][j + 1].material == Empty || grid[i + 1][j + 1].material == Water ||
						grid[i + 1][j + 1].material == Steam) &&
						(grid[i + 1][j].material == Empty || grid[i + 1][j].material == Water || grid[i + 1][j].material == Steam));

					if (canMoveLeft || canMoveRight)
					{
						int newX = i + (int32_t)randomValue;
						if (newX >= 0 && newX < COLS && j + 1 < ROWS &&
							(grid[newX][j].material == Empty || grid[newX][j].material == Water || grid[newX][j].material == Steam))
						{
							// Check if the target position is already occupied
							if (grid[newX][j+1].material == Empty || grid[newX][j+1].material == Water || grid[newX][j+1].material == Steam)
							{
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
				if (grid_duplicate[index][j - 1].material == Empty || 
					grid_duplicate[index][j - 1].material == Water || 
					grid_duplicate[index][j - 1].material == Sand )
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

void updateSpawner(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]) {
    for (int j = ROWS - 2; j >= 0; j--) {
        for (int i = 0; i < COLS; i++) {
            if (grid_duplicate[i][j].material == Spawner) {
                int32_t radius = 1;
                for (int k = -radius; k <= radius; k++) {
                    for (int l = -radius; l <= radius; l++) {
                        int newI = i + k;
                        int newJ = j + l;
                        if (newI >= 0 && newI < COLS && newJ >= 0 && newJ < ROWS) {
                            if (grid[newI][newJ].material == Empty && (float)rand()/RAND_MAX > 0.8f) {
                                grid[newI][newJ].material = grid[i][j].w_material;
                                // Assuming rand_color_mat() returns a Color
                                grid[newI][newJ].color = rand_color_mat(grid[i][j].w_material);
                            }
                        }
                    }
                }
            }
        }
    }
}


void updateVoidTile(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]) {
	    for (int j = ROWS - 2; j >= 0; j--) {
        for (int i = 0; i < COLS; i++) {
            if (grid_duplicate[i][j].material == VoidTile) {
                int32_t radius = 3;
                for (int k = -radius; k <= radius; k++) {
                    for (int l = -radius; l <= radius; l++) {
                        int newI = i + k;
                        int newJ = j + l;
						if (newI == i && newJ == j)
						{
							continue;
						}
                        if (newI >= 0 && newI < COLS && newJ >= 0 && newJ < ROWS) {
                            if (grid[newI][newJ].material != Empty && grid[newI][newJ].material != VoidTile) {
								memset(&grid[newI][newJ], 0, sizeof(Cell));
                                // Assuming rand_color_mat() returns a Color
                            }
                        }
                    }
                }
            }
        }
    }
}


	
