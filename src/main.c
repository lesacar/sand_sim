#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "arg_handler.h"
#include "setup.h"

bool update_should_stop = false;

typedef struct {
    Cell (*grid)[ROWS];
    Cell (*grid_duplicate)[ROWS];
    pthread_mutex_t* grid_mutex;
    pthread_mutex_t* grid_duplicate_mutex;
} UpdateData;

void* update_worker(void* data) {
    UpdateData* update_data = (UpdateData*)data;
    Cell (*grid)[ROWS] = update_data->grid;
    Cell (*grid_duplicate)[ROWS] = update_data->grid_duplicate;
    pthread_mutex_t* grid_mutex = update_data->grid_mutex;
    pthread_mutex_t* grid_duplicate_mutex = update_data->grid_duplicate_mutex;
    
    while (!update_should_stop) {
        // Lock mutexes before modifying grid and grid_duplicate
        pthread_mutex_lock(grid_mutex);
        pthread_mutex_lock(grid_duplicate_mutex);

        // Execute updates
        memcpy(*grid_duplicate, *grid, sizeof(Cell) * COLS * ROWS);
        sand(grid, grid_duplicate);
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
        updateSteam(grid, grid_duplicate);
        memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
        updateWater(grid, grid_duplicate);
        
        // Unlock mutexes after updates
        pthread_mutex_unlock(grid_mutex);
        pthread_mutex_unlock(grid_duplicate_mutex);
        
        // Sleep for 1/60th of a second to maintain 60 updates per second
        usleep(1000000 / 60);
    }
    
    // Cleanup and exit thread
    pthread_exit(NULL);
	return NULL;
}

int main(int argc, char **argv)
{
	// Setup window and display settings
	setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "sand_sim", LOG_INFO, false);
	int32_t current_monitor = handle_arguments(argc, argv);
	if (current_monitor < 0)
	{
		exit(EXIT_FAILURE);
	}
	pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t grid_duplicate_mutex = PTHREAD_MUTEX_INITIALIZER;
	BeginDrawing();
	ClearBackground(BLACK);
	EndDrawing();
	bool pause = false;
	printf("%zu\n", sizeof(Cell));
	bool toggle_fps_cap = true;
	uint32_t material = 0;
	bool brushMode = true;
	
	Cell(*grid)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
	memset(grid, 0, sizeof(Cell) * COLS * ROWS);
	printf("TARGET FPS: %d\n", set_monitor_and_fps(current_monitor));
	// SetConfigFlags(FLAG_MSAA_4X_HINT);
	Shader bloomShader = LoadShader(0, "src/shaders/bloom.fs");

	// Variables for brush size and scroll hint message
	int32_t mscroll_brushSize = 10;
	bool scroll_hint_message = false;

	float cur_dt = 0;
	float second_counter = 0.0f;
	float fps_counter = 0.0f;
	int frame_counter = 0;
	float average_fps = 0.0f;
	// Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, defaultCodepoints, 1);
	Font jetmono = LoadFontEx("./src/fonts/JetBrainsMonoNLNerdFont-Regular.ttf", 20, 0, 251);

	// Create a texture to render the grid
	RenderTexture2D gridTexture = LoadRenderTexture(COLS, ROWS);
	if (gridTexture.id == 0)
	{
		printf("Failed to create grid texture\n");
		exit(EXIT_FAILURE);
	}
	SetTargetFPS(60);
	Cell(*grid_duplicate)[ROWS] = (Cell(*)[ROWS])malloc(sizeof(Cell) * COLS * ROWS);
	if (grid_duplicate == NULL)
	{
		printf("Failed to malloc grid\n");
		return -1;
	}

	int32_t	tileCount = 0;

	pthread_t update_thread;
    UpdateData update_data = { grid, grid_duplicate, &grid_mutex, &grid_duplicate_mutex };
    pthread_create(&update_thread, NULL, update_worker, (void*)&update_data);
	while (!WindowShouldClose())
	{
		cur_dt = GetFrameTime(); // Get frame time
		pthread_mutex_t grid_mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_t grid_duplicate_mutex = PTHREAD_MUTEX_INITIALIZER;
		frame_counter++;
		fps_counter += cur_dt;
		/*if (!pause) {
			memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
			sand(grid, grid_duplicate);
			memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
			updateSteam(grid, grid_duplicate);
			memcpy(grid_duplicate, grid, sizeof(Cell) * COLS * ROWS);
			updateWater(grid, grid_duplicate);
		}*/

		BeginDrawing();
		ClearBackground(BLACK);
		// Activate render texture
		// BeginShaderMode(bloomShader);
			BeginTextureMode(gridTexture);
		ClearBackground(BLACK);

		// uniform vec4 colDiffuse;
		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				if (grid[i][j].material != Empty)
				{
					Color color = grid[i][j].color;
					DrawRectangle(i, j, 1, 1, color);
				}
			}
		}

		// End drawing to render texture
		EndTextureMode();
		DrawTexturePro(gridTexture.texture, (Rectangle){0, 0, (float)gridTexture.texture.width, (float)-gridTexture.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0, WHITE);
		// EndShaderMode();

		//  Update brush size with mouse wheel input
		mscroll_brushSize += (int32_t)GetMouseWheelMove() * 3;
		if (mscroll_brushSize < 1)
			mscroll_brushSize = 1;
		else if (mscroll_brushSize > 150)
			mscroll_brushSize = 150;

		// Count sand blocks for display
		if (!pause) {
			tileCount = 0;
			for (int i = 0; i < COLS; i++)
			{
				for (int j = 0; j < ROWS; j++)
				{
					if (grid[i][j].material != Empty) // TODO: check material > Empty; instead of sand only
					{
						tileCount++;
					}
				}
			}
		}
		// Display FPS and sand block count
		DrawRectangle(15, 15, 580, 50, WHITE);
		/*#ifdef WIN32
		DrawText(TextFormat("Tiles: %05d | Average FPS: %.2f | FPS: %05d ", tileCount, (double)average_fps, (int)(1.0f / cur_dt)), 20, 20, 20, BLACK);
		#else*/
		DrawTextEx(jetmono, TextFormat("Tiles on screen: %05d | Average FPS: %.2f | FPS: %05d ", tileCount, (double)average_fps, (int)(1.0f / cur_dt)), (Vector2){20, 20}, 20, 1, BLACK);
		// #endif
		DrawText(TextFormat("Brush size: %d mode = %s", mscroll_brushSize, brushMode ? "true" : "false"), 20, 44, 20, RED);
		DrawRectangle(15, 140, 200, 50, WHITE);
		DrawRectangle(20, 145, 40, 40, (Color){201, 170, 127, 255});
		DrawRectangle(65, 145, 40, 40, (Color){0, 0, 255, 255});
		DrawRectangle(65 + 45, 145, 40, 40, (Color){51, 83, 69, 255});
		int temp_draw_mouse_x = GetMouseX();
		int temp_draw_mouse_y = GetMouseY();
		if (temp_draw_mouse_x / BLOCK_SIZE > COLS) { temp_draw_mouse_x = COLS;}
		if (temp_draw_mouse_x / BLOCK_SIZE > ROWS) { temp_draw_mouse_y = ROWS;}
		DrawText(TextFormat("M:%d\nC:%u,%u,%u\n", grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].material,
		   grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.r,
		   grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.g,
		   grid[temp_draw_mouse_x/BLOCK_SIZE][temp_draw_mouse_y/BLOCK_SIZE].color.b), SCREEN_WIDTH-100,20,16,WHITE);
		if (IsKeyPressed(KEY_B)) {
			brushMode = !brushMode;
		}
		if(IsKeyPressed(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			int mx = GetMouseX();
			int my = GetMouseY();
			if (my > 145 && my < 185)
			{
				if (mx > 20 && mx < 60)
				{
					material = Sand;
				}
				else if (mx > 65 && mx < 65 + 40)
				{
					material = Water;
				}
				else if (mx > 65 + 45 && mx < 140)
				{
					material = Stone;
				}
				else if (mx > 65 + 90 && mx < 140 + 45)
				{
					material = Steam;
				}
			}
		}

		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			int mx = GetMouseX();
			int my = GetMouseY();
			if (mx > 15 && mx < 15 + 200 && my > 140 && my < 140 + 50)
			{
			}
			else
			{

				int gridX = mx / BLOCK_SIZE;
				int gridY = my / BLOCK_SIZE;

				if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
				{
					spawnSandBrush(grid, mx, my, mscroll_brushSize, material, brushMode);
				}
			}
		}

		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		{
			int mx = GetMouseX();
			int my = GetMouseY();

			int gridX = mx / BLOCK_SIZE;
			int gridY = my / BLOCK_SIZE;

			if (gridX >= 0 && gridX < COLS && gridY >= 0 && gridY < ROWS)
			{
				spawnSandBrush(grid, mx, my, mscroll_brushSize, 0, brushMode);
			}
		}

		// Display scroll hint message
		if (!scroll_hint_message)
		{
			DrawText("Scroll with mouse wheel to increase/reduce brush size (max 50)", 20, 66, 20, YELLOW);
			DrawText("Press R to reset simulation, Press B to toggle BRUSH / CIRCLE draw mode", 20, 88, 20, YELLOW);
			DrawText("Press K to toggle fps cap", 20, 110, 20, YELLOW);
			DrawText("Press H to hide this message", 20, 132, 20, YELLOW); // vertical offset by 22 between texts
			if (IsKeyPressed(KEY_H))
			{
				scroll_hint_message = true;
			}
			
		}
		if (IsKeyPressed(KEY_K))
		{
			if (toggle_fps_cap) {
				SetTargetFPS(5);
				toggle_fps_cap = !toggle_fps_cap;
			}
			else {
				SetTargetFPS(0);
				toggle_fps_cap = !toggle_fps_cap;
			}
		}

		// Reset simulation if R key is pressed
		if (IsKeyPressed(KEY_R))
		{
			memset(grid, 0, sizeof(Cell) * COLS * ROWS);

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
		pthread_mutex_unlock(&grid_mutex);
        pthread_mutex_unlock(&grid_duplicate_mutex);
	}
	update_should_stop = true;
	UnloadShader(bloomShader);
	UnloadRenderTexture(gridTexture);
	free(grid_duplicate);
	CloseWindow();
	return 0;
}
